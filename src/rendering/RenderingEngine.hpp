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
	public:
		RenderingEngine(AbstractGame& _game, const char* _title, int _width, int _height)
			: game(&_game), width(_width), height(_height), title(_title) {};

		int start();

		void _updateSize(int _width, int _height);

	private:
		GLFWwindow* window{ NULL };
		AbstractGame* game;

		int width;
		int height;

		const char* title;

		int init();

		void cleanUp();
	};
}
