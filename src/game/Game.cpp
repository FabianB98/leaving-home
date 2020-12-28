#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/components/Relationship.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "../rendering/Skybox.hpp"
#include "../ui/ToolSelection.hpp"
#include "../ui/Debug.hpp"
#include "../ui/Info.hpp"
#include "components/AxisConstrainedMoveController.hpp"
#include "components/FirstPersonRotateController.hpp"
#include "components/FreeFlyingMoveController.hpp"
#include "components/HeightConstrainedMoveController.hpp"
#include "systems/MovementInputSystem.hpp"
#include "DayNightCycle.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"
#include "world/Resource.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <time.h>

#include <chrono>

namespace game
{
	world::World* wrld;
	entt::entity sun;

	DayNightCycle daynight;
	rendering::Skybox* skybox;
	rendering::shading::Shader* simple;
	rendering::shading::Shader* waterShader;
	rendering::shading::Shader* terrainShader;

	entt::entity defaultCamera;
	entt::entity freeFlightCamera;

	gui::CameraType selectedCamera;
	gui::Tool selectedTool;

	double randomDouble()
	{
		return (double) rand() / (double) RAND_MAX;
	}

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		simple = new rendering::shading::Shader("simpleInstanced");
		waterShader = new rendering::shading::LightSupportingShader("waterInstanced", true);
		terrainShader = new rendering::shading::Shader("deferred/terrain");

		skybox = new rendering::Skybox("skybox", "skybox");

		selectedTool = gui::Tool::BUILD;

		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));

		using namespace rendering::components;

		auto& registry = renderingEngine->getRegistry();

		wrld = new world::World(256, registry, terrainShader, waterShader);
		int worldSize = 8;
		for (int column = -worldSize; column <= 0; column++)
			for (int row = -worldSize - column; row <= worldSize; row++)
				wrld->generateChunk(row, column);
		for (int column = 1; column <= worldSize; column++)
			for (int row = -worldSize; row <= worldSize - column; row++)
				wrld->generateChunk(row, column);

		float width = renderingEngine->getFramebufferWidth();
		float height = renderingEngine->getFramebufferHeight();

		entt::entity cameraBase = registry.create();
		registry.emplace<EulerComponentwiseTransform>(cameraBase, glm::vec3(0, 0, 0), 0, glm::radians(-40.0f), 0, glm::vec3(1.0f));
		registry.emplace<components::HeightConstrainedMoveController>(cameraBase, GLFW_MOUSE_BUTTON_RIGHT, 8.0f);
		registry.emplace<components::FirstPersonRotateController>(cameraBase, GLFW_MOUSE_BUTTON_MIDDLE, 0.2f, glm::radians(-90.0f), glm::radians(-10.0f));
		registry.emplace<rendering::components::Relationship>(cameraBase);

		defaultCamera = registry.create();
		auto parameters = std::make_shared<rendering::components::PerspectiveCameraParameters>(
			glm::radians(45.f), width / height, 1.f, 10000.f);
		registry.emplace<rendering::components::Camera>(defaultCamera, parameters); 
		registry.emplace<EulerComponentwiseTransform>(defaultCamera, glm::vec3(0, 0, 100), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<components::AxisConstrainedMoveController>(defaultCamera, glm::vec3(0, 0, 1), 1000.0f, 50.0f, 1000.0f);
		registry.emplace<rendering::components::Relationship>(defaultCamera);

		rendering::systems::relationship(registry, cameraBase, defaultCamera);


		freeFlightCamera = registry.create();
		parameters = std::make_shared<rendering::components::PerspectiveCameraParameters>(
			glm::radians(45.f), width / height, 1.f, 10000.f);
		registry.emplace<rendering::components::Camera>(freeFlightCamera, parameters);
		registry.emplace<components::FreeFlyingMoveController>(freeFlightCamera, 15.f);
		registry.emplace<components::FirstPersonRotateController>(freeFlightCamera, GLFW_MOUSE_BUTTON_RIGHT);
		registry.emplace<EulerComponentwiseTransform>(freeFlightCamera, glm::vec3(0, 25, 5), 0, 0, 0, glm::vec3(1.0f));


		sun = registry.create();
		registry.emplace<MatrixTransform>(sun, glm::mat4(1.f));
		registry.emplace<DirectionalLight>(sun, glm::vec3(1), glm::vec3(2, 1, 1));
	}
	
	bool pressed;
	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		game::systems::updateMovementInputSystem(renderingEngine, deltaTime, wrld->getHeightGenerator());

		auto io = ImGui::GetIO();
		
		bool pressedNew = renderingEngine->isMouseButtonPressed(GLFW_MOUSE_BUTTON_1) && !io.WantCaptureMouse;
		if (!pressedNew && pressed && selectedTool == gui::Tool::VIEW){
			auto pick = renderingEngine->getPickingResult();
			auto* selected = wrld->getChunkByCompleteCellId(pick)->getCellByCompleteCellId(pick);
			gui::openCellInfo(renderingEngine->getMousePosition(), renderingEngine->getFramebufferSize(), selected);
		}
		pressed = pressedNew;
	}

	bool added = false;
	float time = glfwGetTime();
	void Game::update(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		wrld->addGeneratedChunks();

		auto& registry = renderingEngine->getRegistry();

		daynight.update(deltaTime);
		registry.replace<rendering::components::DirectionalLight>(sun, daynight.getSunColor(), daynight.getSunDirection());

		auto camPointer = selectedCamera == gui::CameraType::DEFAULT ? defaultCamera : freeFlightCamera;
		renderingEngine->setMainCamera(camPointer);


		//if (glfwGetTime() >= time + 10.f && !added) {
		//	added = true;
		//	for (auto c : wrld->getChunks()) {
		//		for (auto cell : c.second->getCells()) {
		//			auto* content = cell.second->getContent();
		//			if (content == nullptr) continue;

		//			//glm::vec3 pos = glm::vec3(content->getTransform().getTransform()[3]);
		//			auto& registry = renderingEngine->getRegistry();
		//			auto light = registry.create();
		//			registry.emplace<rendering::components::MatrixTransform>(light, content->getTransform().getTransform());
		//			registry.emplace<rendering::components::PointLight>(light, glm::vec3(40), glm::vec3(0,5,0));
		//		}
		//	}
		//}
	}
	
	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		float time = daynight.getTime();

		skybox->use();
		skybox->getShader()->setUniformFloat("time", time);
		skybox->getShader()->setUniformVec3("sunColor", daynight.getSunColor());
		skybox->render(renderingEngine);

		gui::renderCellInfo();
		gui::renderDebugWindow(daynight, &selectedCamera);
		gui::renderToolSelection(&selectedTool, renderingEngine->getFramebufferHeight());

	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete wrld;
	}
}
