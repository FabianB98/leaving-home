#include "RenderingEngine.hpp"

namespace rendering 
{	
    RenderingEngine::RenderingEngine(AbstractGame& _game, const char* _title, int _width, int _height)
        : game(&_game), width(_width), height(_height), title(_title)
    {
        auto cameraEntity = registry.create();
        
        auto parameters = std::make_shared<components::PerspectiveCameraParameters>(
            glm::radians(45.f), (float) width / (float) height, .1f, 1000.f);
        registry.emplace<components::Camera>(cameraEntity, parameters);
        registry.emplace<components::MatrixTransform>(cameraEntity, glm::inverse(glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0))));

        mainCamera = cameraEntity;
    }

    void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
        RenderingEngine* engine = (RenderingEngine*) glfwGetWindowUserPointer(window);
        engine->_updateSize(width, height);
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
        wireframeShader = new shading::Shader("simpleInstanced");

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

        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // select default shader for rendering system
        auto defaultShader = showWireframe ? wireframeShader : mainShader;

        auto camera = updateCamera(mainCamera, (float) width / (float) height);
        //rendering::systems::updateLights(registry, *mainShader);
        rendering::systems::renderRenderingSystem(registry, camera, defaultShader, showWireframe);

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
        ImGui::Begin("Debug");

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
