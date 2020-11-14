#include "RenderingEngine.hpp";

namespace rendering 
{	
    void RenderingEngine::frameBufferSizeCallback(GLFWwindow* window, int _width, int _height) {
        width = _width;
        height = _height;

        glViewport(0, 0, width, height);
    }

    void RenderingEngine::start()
    {
        
    }

    void RenderingEngine::init()
    {
        // Initialize GLFW in core profile.
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return;
        }

        // Set up the window hints to use OpenGL 3.3, 4x antialising and the OpenGL core profile (i.e. not the old OpenGL).
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Open a window and create its OpenGL context.
        GLFWwindow* window;
        window = glfwCreateWindow(width, height, "Rendering Engine", NULL, NULL);
        if (window == NULL)
        {
            std::cerr << "Failed to open GLFW window! Does your graphics card support OpenGL 3.3?" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

        // Initialize GLEW in core profile.
        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW!" << std::endl;
            return;
        }
    }

    void RenderingEngine::cleanUp()
    {
        
    }
}
