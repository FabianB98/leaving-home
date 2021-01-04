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
	constexpr auto MSAA_SAMPLES = 2;
	constexpr auto SHADOW_MAP_RES = 2048;

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

	class RenderingEngine : systems::ShaderManager
	{
	public:
		RenderingEngine(AbstractGame& _game, const char* _title, int _width, int _height);

		int start();

		void setUniforms(shading::Shader& shader);

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

		glm::vec2 getScrollDelta()
		{
			return scrollDelta;
		}

		void lockMouseCursorToCenter(bool _lockMouseCursor);

		bool isMouseCursorLockedToCenter()
		{
			return lockMouseCursor;
		}

		int getFramebufferWidth()
		{
			return width;
		}

		int getFramebufferHeight()
		{
			return height;
		}

		glm::vec2 getFramebufferSize()
		{
			return glm::vec2((float) width, (float) height);
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

		void setShadowCamera(entt::entity _shadowCamera)
		{
			shadowCamera = _shadowCamera;
		}

		uint32_t getPickingResult()
		{
			return pickingResult;
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
		glm::vec2 scrollDelta = glm::vec2(0.0f);
		bool lockMouseCursor = false;

		entt::registry registry;

		entt::entity mainCamera;
		entt::entity shadowCamera;

		shading::LightSupportingShader* mainShader;
		shading::Shader* pickingShader;
		shading::Shader* wireframeShader;
		
		shading::Shader* shadowZShader;
		shading::Shader* ssaoShader;
		shading::Shader* ssaoZShader;
		shading::Shader* blurShader;

		shading::Shader* quadShader;
		shading::Shader* screenShader;

		// debug values
		bool showWireframe = false;
		double frameTimeMillis = 0.0;
		int lastFrameCount = 0;
		bool ssaoBlur = false;

		// shadow mapping values
		GLuint shadowBuffer;
		GLuint shadowMap;

		// picking values
		GLuint pickingFramebuffer;
		GLuint pickingDepthbuffer;
		GLuint pickingColorbuffer;
		int index = 0, nextIndex;
		GLuint pbos[2];
		uint32_t pickingResult;

		// deferred rendering values
		GLuint quadVAO;
		GLuint gBuffer;
		GLuint gDepthBuffer;
		GLuint gPosition, gNormal, gAmbient, gDiffuse, gSpecular, gZ;

		// SSAO values
		GLuint ssaoBuffer;
		GLuint ssaoColor;
		GLuint ssaoBlurBufferI;
		GLuint ssaoBlurColorI;
		GLuint ssaoBlurBuffer;
		GLuint ssaoBlurColor;
		GLuint ssaoZBuffer;
		GLuint ssaoZ;

		int init();

		void initShadowMapping();
		void initPicking();
		void initSSAO();
		void initDeferred();

		void input(double deltaTime);

		void update(double deltaTime);

		void setLightingTextureUniforms(shading::Shader& shader);
		void renderScreen(rendering::components::Camera& camera);
		void renderSSAO(rendering::components::Camera& camera);
		void renderSSAOZ(rendering::components::Camera& camera);
		void renderSSAOBlur(GLuint input, bool first);
		void renderQuad(GLuint image);
		void render();

		void doPicking();

		rendering::components::Camera updateCamera(entt::entity cameraEntity, float aspectRatio);

		void renderDebugWindow();

		void cleanUp();

		void _updateSize(int _width, int _height);

		friend void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
		friend void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	};
}
