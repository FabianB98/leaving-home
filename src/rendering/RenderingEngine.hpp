#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// OpenGL related headers
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

// Our headers
#include "AbstractGame.hpp"

namespace rendering 
{	
	class RenderingEngine
	{
	private:
		GLFWwindow* window;
		AbstractGame* game;

		int width;
		int height;

		void init();

		void cleanUp();

		void frameBufferSizeCallback(GLFWwindow* window, int _width, int _height);

	public:
		RenderingEngine(AbstractGame& _game, int _width, int _height) : game(&_game), width(_width) {};

		void start();
	};
}
