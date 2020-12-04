#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// OpenGL related headers
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

// Headers related to the entity component system.
#include <entt/entt.hpp>

// Our headers
#include "components/Camera.hpp"
#include "components/MeshRenderer.hpp"
#include "components/Transform.hpp"
#include "systems/RenderingSystem.hpp"
#include "systems/TransformConversionSystem.hpp"
#include "systems/TransformHierarchySystem.hpp"
#include "../ui/GUI.hpp"

namespace rendering 
{	
	class RenderingEngine;

	class AbstractGame
	{
	public:
		AbstractGame() {};
		~AbstractGame() {};

		virtual void init(RenderingEngine* renderingEngine) = 0;

		virtual void input(RenderingEngine* renderingEngine, double deltaTime) = 0;

		virtual void update(RenderingEngine* renderingEngine, double deltaTime) = 0;

		virtual void render(RenderingEngine* renderingEngine) = 0;

		virtual void cleanUp(RenderingEngine* renderingEngine) = 0;
	};

	class RenderingEngine
	{
	public:
		RenderingEngine(AbstractGame& _game, const char* _title, int _width, int _height);

		int start();

		bool isKeyPressed(int keyCode);

		bool isMouseButtonPressed(int keyCode);

		glm::vec2 getMousePosition()
		{
			return mousePosition;
		}

		glm::vec2 getMouseDelta()
		{
			return mouseDelta;
		}

		void lockMouseCursorToCenter(bool _lockMouseCursor);

		bool isMouseCursorLockedToCenter()
		{
			return lockMouseCursor;
		}

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

		entt::registry& getRegistry()
		{
			return registry;
		}

		entt::entity getMainCamera()
		{
			return mainCamera;
		}

		void setMainCamera(entt::entity _mainCamera)
		{
			mainCamera = _mainCamera;
		}

	private:
		GLFWwindow* window{ NULL };
		AbstractGame* game;

		int width;
		int height;

		const char* title;

		glm::vec4 clearColor = glm::vec4(0.0f);

		glm::vec2 mousePosition = glm::vec2(0.0f);
		glm::vec2 mouseDelta = glm::vec2(0.0f);
		bool lockMouseCursor = false;

		entt::registry registry;

		entt::entity mainCamera;

		shading::LightSupportingShader* mainShader;
		shading::Shader* wireframeShader;

		// debug values
		bool showWireframe = false;
		double frameTimeMillis = 0.0;
		int lastFrameCount = 0;

		int init();

		void input(double deltaTime);

		void update(double deltaTime);

		void render();

		rendering::components::Camera updateCamera(entt::entity cameraEntity, shading::Shader& shader, float aspectRatio);

		void renderDebugWindow();

		void cleanUp();
	};
}
