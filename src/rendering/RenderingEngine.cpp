#include "RenderingEngine.hpp"

namespace rendering 
{	
    RenderingEngine::RenderingEngine(AbstractGame& _game, const char* _title, int _width, int _height)
        : game(&_game), width(_width), height(_height), title(_title)
    {
        auto cameraEntity = registry.create();
        
        auto parameters = std::make_shared<components::PerspectiveCameraParameters>(
            glm::radians(45.f), (float) width / (float) height, .1f, 1000.f);
        auto cameraComponent = registry.emplace<components::Camera>(cameraEntity, parameters);
        auto transformComponent = registry.emplace<components::Transform>(cameraEntity, glm::inverse(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))));

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

        // Perform the actual update loop.
        while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
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
                double frameTimeMillis = 1000.0 / double(numFrames);
                std::cout << frameTimeMillis << " ms/frame, " << numFrames << " FPS" << std::endl;
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

        // Initialize shaders
        mainShader = new shading::LightSupportingShader("phong");
        wireframeShader = new shading::Shader("simple");

        return 0;
    }

    void RenderingEngine::input(double deltaTime)
    {
        glfwPollEvents();
        game->input(this, deltaTime);
    }

    void RenderingEngine::update(double deltaTime)
    {
        game->update(this, deltaTime);
    }

    void RenderingEngine::render()
    {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainShader->use();

        //START OF TEMPORARY CODE FOR TESTING...
        rendering::DirectionalLight light(glm::vec3(2), glm::vec3(2,1,1));
		mainShader->setUniformDirectionalLight("directionalLight", light);

		rendering::PointLight pLight(glm::vec3(0,0,1), glm::vec3(-1.5,-1.5,1.5));
        mainShader->setUniformPointLight("pointLights[0]", pLight);

		pLight.setIntensity(glm::vec3(1, 0, 0));
		pLight.setPosition(glm::vec3(-1.5, 1.5, 1.5));
        mainShader->setUniformPointLight("pointLights[1]", pLight);
        //END OF TEMPORARY CODE FOR TESTING...

        auto camera = updateCamera(mainCamera, *mainShader, (float) width / (float) height);
        auto activeShader = mainShader;

        auto meshes = registry.view<components::MeshRenderer, components::Transform>();
        for (auto entity : meshes) {
            auto [meshRenderer, transform] = registry.get<components::MeshRenderer, components::Transform>(entity);

            meshRenderer.render(*mainShader, transform.getTransform(), camera.getViewProjectionMatrix());
        }

        game->render(this);

        glfwSwapBuffers(window);
    }

    components::Camera RenderingEngine::updateCamera(
        entt::entity cameraEntity, shading::Shader& shader, float aspectRatio)
    {
        using namespace rendering::components;

        auto cameraComponent = registry.get<Camera>(cameraEntity);
        auto transformComponent = registry.get<Transform>(cameraEntity);

        cameraComponent.setAspectRatio(aspectRatio);
        cameraComponent.updateViewProjection(transformComponent.getTransform(), shader);

        return cameraComponent;
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
        glfwTerminate();

        delete mainShader;
        delete wireframeShader;
    }
}
