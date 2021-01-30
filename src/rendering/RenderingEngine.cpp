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

        ImGui::Checkbox("SSAO Blur", &ssaoBlur);
        ImGui::Checkbox("FXAA", &useFXAA);

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

    void RenderingEngine::updateFramebufferTexture(GLuint* location, GLuint data, GLuint storage, unsigned int width, unsigned int height)
    {
        GLuint target = MSAA_SAMPLES == 0 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
        glBindTexture(target, *location);

        if (MSAA_SAMPLES == 0) glTexImage2D(target, 0, data, width, height, 0, GL_RGBA, storage, NULL);
        else glTexImage2DMultisample(target, MSAA_SAMPLES, data, width, height, GL_TRUE);
    }

    void RenderingEngine::updateFramebufferDepthbuffer(GLuint* location, unsigned int width, unsigned int height)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, *location);
        if (MSAA_SAMPLES == 0)
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        else
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH_COMPONENT, width, height);
    }

    void RenderingEngine::_updateSize(int _width, int _height)
    {
        width = _width;
        height = _height;

        glBindTexture(GL_TEXTURE_2D, pickingColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glBindRenderbuffer(GL_RENDERBUFFER, pickingDepthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

        updateFramebufferTexture(&gPosition, GL_RGBA16F, GL_FLOAT, width, height);
        updateFramebufferTexture(&gNormal, GL_RGBA16F, GL_FLOAT, width, height);
        updateFramebufferTexture(&gAmbient, GL_RGBA, GL_UNSIGNED_BYTE, width, height);
        updateFramebufferTexture(&gDiffuse, GL_RGBA, GL_UNSIGNED_BYTE, width, height);
        updateFramebufferTexture(&gSpecular, GL_RGBA, GL_UNSIGNED_BYTE, width, height);
        updateFramebufferTexture(&gZ, GL_R32F, GL_FLOAT, width, height);

        updateFramebufferDepthbuffer(&gDepthBuffer, width, height);

        glBindTexture(GL_TEXTURE_2D, ssaoColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, ssaoZ);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColorI);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);

        updateFramebufferTexture(&mainColor, GL_RGBA, GL_UNSIGNED_BYTE, width, height);
        updateFramebufferDepthbuffer(&mainDepth, width, height);

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

        delete pickingShader;

        delete shadowZShader;
        delete ssaoShader;
        delete ssaoZShader;
        delete blurShader;

        delete quadShader;
        delete screenShader;
        delete fxaaShader;
    }
}
