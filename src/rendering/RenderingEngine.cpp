#include "RenderingEngine.hpp"

namespace rendering 
{	
    RenderingEngine::RenderingEngine(AbstractGame& _game, const char* _title, int _width, int _height)
        : game(&_game), width(_width), height(_height), title(_title)
    {
        
    }

    void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
        RenderingEngine* engine = (RenderingEngine*) glfwGetWindowUserPointer(window);
        engine->_updateSize(width, height);
    }

    void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
    {
        RenderingEngine* engine = (RenderingEngine*)glfwGetWindowUserPointer(window);
        engine->scrollDelta += glm::vec2(xOffset, yOffset);
    }

    int RenderingEngine::start()
    {
        // Try to create a window.
        int initResult = init();
        if (initResult != 0)
        {
            // Couldn't create a window. Abort.
            return initResult;
        }

        // Initialize the game.
        game->init(this);

        // Set up the input devices.
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
        glfwSetScrollCallback(window, scrollCallback);

        // The last time when the update loop was called. Used for calculating the time delta.
        double lastLoopTime = glfwGetTime();

        // Some variables used for logging the FPS and average frame times to the console.
        double lastFpsTime = glfwGetTime(); // The last time when the FPS were logged to the console.
        int numFrames = 0;                  // The amount of frames rendered since the last FPS log.

        // Initialize some last OpenGL flags. These are important for rendering, but don't need to be set every frame.
        glEnable(GL_DEPTH_TEST);    // Enable the depth test.
        glDepthFunc(GL_LESS);       // Discard fragments further away from the camera than the previous fragment.
        glEnable(GL_CULL_FACE);     // Enable face culling (i.e. only render faces pointing to the outside of meshes).
        glEnable(GL_BLEND);         // Enable blending.
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Use standard alpha blending.

        // Perform the actual update loop.
        while (!isKeyPressed(GLFW_KEY_ESCAPE) && glfwWindowShouldClose(window) == 0)
        {
            // Compute time difference between the current and the last frame.
            double currentTime = glfwGetTime();
            float deltaTime = float(currentTime - lastLoopTime);
            lastLoopTime = currentTime;

            // Update the FPS counter.
            numFrames++;
            if (currentTime - lastFpsTime >= 1.0)
            {
                // Only print the FPS once per second.
                frameTimeMillis = 1000.0 / double(numFrames);
                //std::cout << frameTimeMillis << " ms/frame, " << numFrames << " FPS" << std::endl;
                lastFrameCount = numFrames;
                numFrames = 0;
                lastFpsTime += 1.0;
            }

            // Perform the actual game logic.
            input(deltaTime);
            update(deltaTime);
            render();
        }

        // Clean everything up (i.e. destroy the window).
        game->cleanUp(this);
        cleanUp();

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
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1080);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingDepthbuffer);

        glGenBuffers(2, pbos);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[0]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 4, 0, GL_STREAM_READ);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[1]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 4, 0, GL_STREAM_READ);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

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
        glfwWindowHint(GLFW_SAMPLES, 4);
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
        rendering::systems::initRenderingSystem(registry);

        // Initialize gui
        gui::init(window);

        // Initialize shaders
        mainShader = new shading::LightSupportingShader("phongInstanced");
        pickingShader = new shading::Shader("pickingInstanced");
        wireframeShader = new shading::Shader("simpleInstanced");

        initPicking();

        return 0;
    }

    void RenderingEngine::input(double deltaTime)
    {
        glfwPollEvents();

        // Get the current mouse position and determine the mouse movement since the last frame.
        double newPosX, newPosY;
        glfwGetCursorPos(window, &newPosX, &newPosY);
        glm::vec2 newPos = glm::vec2((float)newPosX, (float)newPosY);
        if (lockMouseCursor)
        {
            // Mouse cursor is locked to the center of the window. Movement is therefore calculated as the difference
            // between the current mouse position and the center of the window.
            glm::vec2 centerPos = glm::vec2(width / 2.0f, height / 2.0f);
            glfwSetCursorPos(window, centerPos.x, centerPos.y);
            mouseDelta = newPos - centerPos;
        }
        else
        {
            // Mouse cursor is not locked to the center of the window. Movement is therefore calculated as the
            // difference between the new position and the old position.
            mouseDelta = newPos - mousePosition;
        }
        mousePosition = newPos;

        game->input(this, deltaTime);

        // Reset the scroll delta back to zero so that we can accumulate the scroll inputs until the next frame.
        scrollDelta = glm::vec2(0.0f, 0.0f);
    }

    void RenderingEngine::update(double deltaTime)
    {
        game->update(this, deltaTime);

        rendering::systems::updateTransformConversion(registry);
        rendering::systems::updateHierarchy(registry);
    }

    void RenderingEngine::render()
    {
        // prepare gui for render
        gui::startFrame();

        // select default shader for rendering system
        auto defaultShader = showWireframe ? wireframeShader : mainShader;

        auto camera = updateCamera(mainCamera, (float) width / (float) height);
        rendering::systems::renderRenderingSystemTransforms(registry, camera, defaultShader, showWireframe);

        glBindFramebuffer(GL_FRAMEBUFFER, pickingFramebuffer);
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glViewport(0, 0, getFramebufferWidth(), getFramebufferHeight());

        rendering::systems::renderRenderingSystemPicking(registry, camera, pickingShader);

        // read picking framebuffer
        index = (index + 1) % 2;
        nextIndex = (index + 1) % 2;
        glNamedFramebufferReadBuffer(pickingFramebuffer, GL_FRONT);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[index]);
        double posX, posY;
        glfwGetCursorPos(window, &posX, &posY);
        if (posX <= getFramebufferWidth() && posY <= getFramebufferHeight() && posX >= 0 && posY >= 0) {
            glReadPixels((int)posX, getFramebufferHeight() - (int)posY, 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, 0);

            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[nextIndex]);
            GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
            if (ptr) {
                GLubyte blue = ptr[0];
                GLubyte green = ptr[1];
                GLubyte red = ptr[2];
                pickingResult = ((uint32_t)blue) + (((uint32_t)green) << 8) + (((uint32_t)red) << 16);
                glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            }
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        }
        else {
            pickingResult = 0xffffff;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glViewport(0, 0, getFramebufferWidth(), getFramebufferHeight());

        rendering::systems::renderRenderingSystemForward(registry, camera, pickingResult);
        //rendering::systems::renderRenderingSystemPicking(registry, camera, pickingShader);



        game->render(this);
        renderDebugWindow();
        gui::render();

        glfwSwapBuffers(window);
    }

    components::Camera RenderingEngine::updateCamera(entt::entity cameraEntity, float aspectRatio)
    {
        using namespace rendering::components;

        auto& cameraComponent = registry.get<Camera>(cameraEntity);
        auto transformComponent = registry.get<MatrixTransform>(cameraEntity);
        auto relationship = registry.try_get<components::Relationship>(cameraEntity);

        glm::mat4& cameraTransform = relationship ? relationship->totalTransform : transformComponent.getTransform();

        cameraComponent.setAspectRatio(aspectRatio);
        cameraComponent.updateViewProjection(cameraTransform);

        return cameraComponent;
    }

    void toggleWireframe(bool enable)
    {
        // set draw mode and culling
        GLuint polyMode = enable ? GL_LINE : GL_FILL;
        glPolygonMode(GL_FRONT_AND_BACK, polyMode);

        if (enable) glDisable(GL_CULL_FACE);
        else glEnable(GL_CULL_FACE);
    }

    void RenderingEngine::renderDebugWindow()
    {
        ImGui::Begin("Rendering");

        ImGui::Text("%f ms / frame, %i FPS", frameTimeMillis, lastFrameCount);
        if (ImGui::Checkbox("Show wireframe", &showWireframe))
            toggleWireframe(showWireframe);

        ImGui::End();
    }

    bool RenderingEngine::isKeyPressed(int keyCode)
    {
        return glfwGetKey(window, keyCode) == GLFW_PRESS;
    }

    bool RenderingEngine::isMouseButtonPressed(int keyCode)
    {
        return glfwGetMouseButton(window, keyCode) == GLFW_PRESS;
    }

    void RenderingEngine::lockMouseCursorToCenter(bool _lockMouseCursor)
    {
        if (_lockMouseCursor == lockMouseCursor)
            return;

        if (_lockMouseCursor)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window, width / 2.0, height / 2.0);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        lockMouseCursor = _lockMouseCursor;
    }

    void RenderingEngine::_updateSize(int _width, int _height)
    {
        width = _width;
        height = _height;

        glBindTexture(GL_TEXTURE_2D, pickingColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glBindRenderbuffer(GL_RENDERBUFFER, pickingDepthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

        glViewport(0, 0, _width, _height);
        render();
    }

    void RenderingEngine::cleanUp()
    {
        systems::cleanUpRenderingSystem(registry);
        systems::cleanUpHierarchySystem(registry);
        gui::cleanUp();
        glfwTerminate();

        delete mainShader;
        delete wireframeShader;
    }
}
