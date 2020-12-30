#include "RenderingEngine.hpp"

namespace rendering
{
    int RenderingEngine::init()
    {
        // Initialize GLFW in core profile.
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return -1;
        }

        // Set up the window hints to use OpenGL 3.3, 4x antialising and the OpenGL core profile (i.e. not the old OpenGL).
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Open a window and create its OpenGL context.
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (window == NULL)
        {
            std::cerr << "Failed to open GLFW window! Does your graphics card support OpenGL 3.3?" << std::endl;
            cleanUp();
            return -2;
        }
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
        glfwSwapInterval(0);    // enable vsync

        // Initialize GLEW in core profile.
        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW!" << std::endl;
            cleanUp();
            return -3;
        }

        // Initialize the rendering and hierarchy system
        rendering::systems::initHierarchySystem(registry);
        rendering::systems::initRenderingSystem(registry, this);

        // Initialize gui
        gui::init(window);

        // Initialize shaders
        mainShader = new shading::LightSupportingShader("deferred/default");
        pickingShader = new shading::Shader("pickingInstanced");
        wireframeShader = new shading::Shader("simpleInstanced");

        screenShader = new shading::LightSupportingShader("deferred/screen");
        quadShader = new shading::Shader("deferred/quad");
        ssaoShader = new shading::SSAOShader("deferred/ssao");
        blurShader = new shading::Shader("deferred/blur");

        initPicking();
        initSSAO();
        initDeferred();

        return 0;
    }

    void RenderingEngine::initPicking()
    {
        glGenFramebuffers(1, &pickingFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, pickingFramebuffer);

        glGenTextures(1, &pickingColorbuffer);
        glBindTexture(GL_TEXTURE_2D, pickingColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getFramebufferWidth(), getFramebufferHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // attach it to currently bound framebuffer object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingColorbuffer, 0);

        glGenRenderbuffers(1, &pickingDepthbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, pickingDepthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getFramebufferWidth(), getFramebufferHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingDepthbuffer);

        glGenBuffers(2, pbos);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[0]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 4, 0, GL_STREAM_READ);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[1]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 4, 0, GL_STREAM_READ);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    void RenderingEngine::initDeferred()
    {
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gPosition);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA16F, getFramebufferWidth(), getFramebufferHeight(), GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, gPosition, 0);

        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gNormal);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA16F, getFramebufferWidth(), getFramebufferHeight(), GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, gNormal, 0);

        glGenTextures(1, &gAmbient);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gAmbient);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA, getFramebufferWidth(), getFramebufferHeight(), GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, gAmbient, 0);

        glGenTextures(1, &gDiffuse);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gDiffuse);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA, getFramebufferWidth(), getFramebufferHeight(), GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D_MULTISAMPLE, gDiffuse, 0);

        glGenTextures(1, &gSpecular);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gSpecular);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGBA, getFramebufferWidth(), getFramebufferHeight(), GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D_MULTISAMPLE, gSpecular, 0);

        GLuint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
        glDrawBuffers(5, attachments);

        glGenRenderbuffers(1, &gDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepthBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH_COMPONENT, getFramebufferWidth(), getFramebufferHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthBuffer);

        glGenVertexArrays(1, &quadVAO);
    }

    void generateNoise(std::vector<glm::vec3>& noise, unsigned int size)
    {
        std::uniform_real_distribution<float> randomFloats(0.f, 1.f);
        std::default_random_engine generator;

        for (unsigned int i = 0; i < size; ++i)
        {
            glm::vec3 value(
                randomFloats(generator) * 2.f - 1.f, 
                randomFloats(generator) * 2.f - 1.f,
                0.f
            );
            noise.push_back(value);
        }
    }

    float lerpf(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    void generateKernel(std::vector<glm::vec3>& kernel, unsigned int size)
    {
        std::uniform_real_distribution<float> randomFloats(0.f, 1.f);
        std::default_random_engine generator;

        for (unsigned int i = 0; i < size; ++i) {
            glm::vec3 sample(
                randomFloats(generator) * 2.f - 1.f,
                randomFloats(generator) * 2.f - 1.f,
                randomFloats(generator)
            );
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            
            float scale = float(i) / 64.f;
            scale = lerpf(0.1f, 1.f, scale * scale);
            sample *= scale;
            kernel.push_back(sample);
        }
    }

    void RenderingEngine::initSSAO()
    {
        unsigned int noiseSize = ssaoShader->getNoiseSize();
        unsigned int kernelSize = ssaoShader->getKernelSize();

        generateNoise(ssaoNoise, noiseSize * noiseSize);
        generateKernel(ssaoKernel, kernelSize);

        glGenTextures(1, &ssaoNoiseTexture);
        glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, noiseSize, noiseSize, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenFramebuffers(1, &ssaoBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);

        glGenTextures(1, &ssaoColor);
        glBindTexture(GL_TEXTURE_2D, ssaoColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getFramebufferWidth(), getFramebufferHeight(), 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColor, 0);


        glGenFramebuffers(1, &ssaoBlurBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);

        glGenTextures(1, &ssaoBlurColor);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getFramebufferWidth(), getFramebufferHeight(), 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColor, 0);
    }
}