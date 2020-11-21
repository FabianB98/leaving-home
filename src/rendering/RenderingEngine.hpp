#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// OpenGL related headers
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

namespace rendering 
{	
	class RenderingEngine;

	class AbstractGame
	{
	public:
		AbstractGame() {};
		~AbstractGame() {};

		virtual void init(RenderingEngine* renderingEngine) = 0;

		virtual void input(double deltaTime) = 0;

		virtual void update(double deltaTime) = 0;

		virtual void render(RenderingEngine* renderingEngine) = 0;

		virtual void cleanUp() = 0;
	};

	class RenderingEngine
	{
	public:
		RenderingEngine(AbstractGame& _game, const char* _title, int _width, int _height)
			: game(&_game), width(_width), height(_height), title(_title) {};

		int start();

		void _updateSize(int _width, int _height);

		int getFramebufferWidth()
		{
			return width;
		}

		int getFrameBufferHeight()
		{
			return height;
		}

		void setClearColor(glm::vec4 _clearColor)
		{
			clearColor = _clearColor;
		}

	private:
		GLFWwindow* window{ NULL };
		AbstractGame* game;

		int width;
		int height;

		const char* title;

		glm::vec4 clearColor = glm::vec4(0.0f);

		int init();

		void input(double deltaTime);

		void update(double deltaTime);

		void render();

		void cleanUp();
	};
}
