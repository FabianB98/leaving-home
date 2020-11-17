#include "RenderingEngine.hpp"

namespace rendering 
{	
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
        game->init();

        // Set up the input devices.
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

        // The last time when the update loop was called. Used for calculating the time delta.
        double lastLoopTime = glfwGetTime();

        // Some variables used for logging the FPS and average frame times to the console.
        double lastFpsTime = glfwGetTime(); // The last time when the FPS were logged to the console.
        int numFrames = 0;                  // The amount of frames rendered since the last FPS log.

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
        game->cleanUp();
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

        // Initialize GLEW in core profile.
        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW!" << std::endl;
            cleanUp();
            return -3;
        }

        return 0;
    }

    void RenderingEngine::input(double deltaTime)
    {
        glfwPollEvents();
        game->input(deltaTime);
    }

    void RenderingEngine::update(double deltaTime)
    {
        game->update(deltaTime);
    }

    void RenderingEngine::render()
    {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game->render();

        glfwSwapBuffers(window);
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
    }
}
