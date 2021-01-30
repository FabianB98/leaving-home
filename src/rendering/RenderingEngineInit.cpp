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
        fxaaShader = new shading::Shader("deferred/fxaa");

        initShadowMapping();
        initPicking();
        initSSAO();
        initDeferred();

        return 0;
    }

    void RenderingEngine::initShadowMapping()
    {
        glGenFramebuffers(MAX_SHADOW_MAPS, &shadowBuffers[0]);
        glGenTextures(MAX_SHADOW_MAPS, &shadowMaps[0]);
        
        for (unsigned int i = 0; i < MAX_SHADOW_MAPS; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffers[i]);
            glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_RES, SHADOW_MAP_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[i], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
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

    void RenderingEngine::createFramebufferTexture(GLuint* location, GLuint data, GLuint storage, unsigned int attachment)
    {
        GLuint target = MSAA_SAMPLES == 0 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
        auto width = getFramebufferWidth();
        auto height = getFramebufferHeight();

        glGenTextures(1, location);
        glBindTexture(target, *location);
        if (MSAA_SAMPLES == 0)
            glTexImage2D(target, 0, data, width, height, 0, GL_RGBA, storage, NULL);
        else
            glTexImage2DMultisample(target, MSAA_SAMPLES, data, width, height, GL_TRUE);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, target, *location, 0);
    }

    void RenderingEngine::createFramebufferDepthbuffer(GLuint* location)
    {
        auto width = getFramebufferWidth();
        auto height = getFramebufferHeight();

        glGenRenderbuffers(1, location);
        glBindRenderbuffer(GL_RENDERBUFFER, *location);
        if (MSAA_SAMPLES == 0)
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        else
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *location);
    }

    void RenderingEngine::initDeferred()
    {
        auto width = getFramebufferWidth();
        auto height = getFramebufferHeight();

        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        createFramebufferTexture(&gPosition, GL_RGBA16F, GL_FLOAT, 0);
        createFramebufferTexture(&gNormal, GL_RGBA16F, GL_FLOAT, 1);
        createFramebufferTexture(&gAmbient, GL_RGBA, GL_UNSIGNED_BYTE, 2);
        createFramebufferTexture(&gDiffuse, GL_RGBA, GL_UNSIGNED_BYTE, 3);
        createFramebufferTexture(&gSpecular, GL_RGBA, GL_UNSIGNED_BYTE, 4);
        createFramebufferTexture(&gZ, GL_R32F, GL_FLOAT, 5);

        GLuint attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
        glDrawBuffers(6, attachments);

        createFramebufferDepthbuffer(&gDepthBuffer);

        glGenVertexArrays(1, &quadVAO);


        glGenFramebuffers(1, &mainBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mainBuffer);

        createFramebufferTexture(&mainColor, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        createFramebufferDepthbuffer(&mainDepth);

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