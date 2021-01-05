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
        glfwSwapInterval(1);    // enable vsync

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
        shadowZShader = new shading::Shader("deferred/shadowZ");
        ssaoShader = new shading::Shader("deferred/ssao");
        ssaoZShader = new shading::Shader("deferred/ssaoZ");
        blurShader = new shading::Shader("deferred/blur");

        initShadowMapping();
        initPicking();
        initSSAO();
        initDeferred();

        return 0;
    }

    void RenderingEngine::initShadowMapping()
    {
        glGenFramebuffers(1, &shadowBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);

        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_RES, SHADOW_MAP_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
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

        glGenTextures(1, &gZ);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gZ);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_R32F, getFramebufferWidth(), getFramebufferHeight(), GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D_MULTISAMPLE, gZ, 0);

        GLuint attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
        glDrawBuffers(6, attachments);

        glGenRenderbuffers(1, &gDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, gDepthBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH_COMPONENT, getFramebufferWidth(), getFramebufferHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthBuffer);

        glGenVertexArrays(1, &quadVAO);
    }

    void RenderingEngine::initSSAO()
    {
        glGenFramebuffers(1, &ssaoZBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoZBuffer);

        glGenTextures(1, &ssaoZ);
        glBindTexture(GL_TEXTURE_2D, ssaoZ);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, getFramebufferWidth(), getFramebufferHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoZ, 0);



        glGenFramebuffers(1, &ssaoBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);

        glGenTextures(1, &ssaoColor);
        glBindTexture(GL_TEXTURE_2D, ssaoColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getFramebufferWidth(), getFramebufferHeight(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColor, 0);


        glGenFramebuffers(1, &ssaoBlurBufferI);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBufferI);

        glGenTextures(1, &ssaoBlurColorI);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColorI);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getFramebufferWidth(), getFramebufferHeight(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColorI, 0);

        glGenFramebuffers(1, &ssaoBlurBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);

        glGenTextures(1, &ssaoBlurColor);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, getFramebufferWidth(), getFramebufferHeight(), 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColor, 0);
    }
}