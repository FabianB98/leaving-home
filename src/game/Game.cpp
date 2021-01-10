#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/components/Relationship.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "../rendering/components/Shadow.hpp"
#include "../rendering/Skybox.hpp"
#include "../ui/ToolSelection.hpp"
#include "../ui/Debug.hpp"
#include "../ui/Info.hpp"
#include "components/AxisConstrainedMoveController.hpp"
#include "components/FirstPersonRotateController.hpp"
#include "components/FreeFlyingMoveController.hpp"
#include "components/HeightConstrainedMoveController.hpp"
#include "systems/MovementInputSystem.hpp"
#include "systems/ResourceProcessingSystem.hpp"
#include "DayNightCycle.hpp"
#include "PickingChunkSelection.hpp"
#include "world/Building.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"
#include "world/Resource.hpp"
#include "world/Constants.hpp"

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
	entt::entity starlight;

	DayNightCycle daynight;
	rendering::Skybox* skybox;
	rendering::shading::Shader* simple;
	rendering::shading::Shader* waterShader;
	rendering::shading::Shader* terrainShader;

	entt::entity cameraBase;
	entt::entity defaultCamera;
	entt::entity freeFlightCamera;

	entt::entity shadowCamera;
	entt::entity shadowCamera1;

	gui::CameraType selectedCamera;
	gui::Tool selectedTool;
	gui::Building selectedBuilding;

	rendering::model::Mesh* tree;

	std::shared_ptr<rendering::components::OrthographicCameraParameters> shadowCamParams;

	double randomDouble()
	{
		return (double) rand() / (double) RAND_MAX;
	}

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		simple = new rendering::shading::Shader("simpleInstanced");
		waterShader = new rendering::shading::LightSupportingShader("waterInstanced", true);
		terrainShader = new rendering::shading::Shader("deferred/terrain");

		tree = new rendering::model::Mesh("tree");

		skybox = new rendering::Skybox("skybox", "skybox");

		selectedTool = gui::Tool::BUILD;
		selectedBuilding = gui::Building::TEST_BUILDING;

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

		world::Drone::spawnNewDrone(registry, glm::vec3(0.0f, 0.0f, 0.0f));
		registry.view<world::Drone>().each([](auto entity, auto& drone) {
			drone.inventory.addItemTyped<world::Wood>(10.0f);
		});

		float width = renderingEngine->getFramebufferWidth();
		float height = renderingEngine->getFramebufferHeight();

		cameraBase = registry.create();
		registry.emplace<EulerComponentwiseTransform>(cameraBase, glm::vec3(0, 0, 0), 0, glm::radians(-40.0f), 0, glm::vec3(1.0f));
		registry.emplace<components::HeightConstrainedMoveController>(cameraBase, GLFW_MOUSE_BUTTON_RIGHT, 0.2f);
		registry.emplace<components::FirstPersonRotateController>(cameraBase, GLFW_MOUSE_BUTTON_MIDDLE, 0.005f, glm::radians(-90.0f), glm::radians(-10.0f));
		registry.emplace<rendering::components::Relationship>(cameraBase);

		defaultCamera = registry.create();
		auto parameters = std::make_shared<rendering::components::PerspectiveCameraParameters>(
			glm::radians(45.f), width / height, 1.f, 10000.f);
		registry.emplace<rendering::components::Camera>(defaultCamera, parameters); 
		registry.emplace<EulerComponentwiseTransform>(defaultCamera, glm::vec3(0, 0, 100), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<components::AxisConstrainedMoveController>(defaultCamera, glm::vec3(0, 0, 1), 20.0f, 50.0f, 1000.0f);
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

		starlight = registry.create();
		registry.emplace<MatrixTransform>(starlight, glm::mat4(1.f));
		registry.emplace<DirectionalLight>(starlight, glm::vec3(1), glm::vec3(2, 1, 1));

		shadowCamera = registry.create();
		shadowCamParams = std::make_shared<rendering::components::OrthographicCameraParameters>(
			100.f, 1.f, 400.f, -400.f);
		registry.emplace<rendering::components::MatrixTransform>(shadowCamera, glm::mat4(1));
		registry.emplace<rendering::components::Camera>(shadowCamera, shadowCamParams);
		registry.emplace<rendering::components::CastShadow>(shadowCamera, sun);

		shadowCamera1 = registry.create();
		auto shadowCamParams1 = std::make_shared<rendering::components::OrthographicCameraParameters>(
			1800.f, 1.f, 1.f, -1500.f);
		registry.emplace<rendering::components::MatrixTransform>(shadowCamera1, glm::mat4(1));
		registry.emplace<rendering::components::Camera>(shadowCamera1, shadowCamParams1);
		registry.emplace<rendering::components::CastShadow>(shadowCamera1, sun);


		renderingEngine->setShadowCameras(std::vector<entt::entity>{ shadowCamera, shadowCamera1 });
	}

	void placeBuilding(world::Cell* cell, gui::Building toPlace)
	{
		if (toPlace == gui::Building::TEST_BUILDING)
			cell->placeBuilding<world::TestBuilding>();
		else if (toPlace == gui::Building::OTHER_TEST_BUILDING)
			cell->placeBuilding<world::OtherTestBuilding>();
	}
	
	bool pressed;
	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		game::systems::updateMovementInputSystem(renderingEngine, deltaTime, wrld->getHeightGenerator());

		auto io = ImGui::GetIO();
		
		bool pressedNew = renderingEngine->isMouseButtonPressed(GLFW_MOUSE_BUTTON_1) && !io.WantCaptureMouse;
		if (!pressedNew && pressed)
		{
			auto pick = renderingEngine->getPickingResult();
			auto* selectedChunk = wrld->getChunkByCompleteCellId(pick);
			if (selectedChunk != nullptr)
			{
				auto* selected = selectedChunk->getCellByCompleteCellId(pick);

				if (selectedTool == gui::Tool::VIEW)
					gui::openCellInfo(renderingEngine->getMousePosition(), renderingEngine->getFramebufferSize(), selected);
				else if (selectedTool == gui::Tool::BUILD && selected != nullptr)
					placeBuilding(selected, selectedBuilding);
				else if (selectedTool == gui::Tool::REMOVE && selected != nullptr)
					selected->setContent(nullptr);
			}
		}
		pressed = pressedNew;
	}

	bool added = false;
	double time = glfwGetTime();
	void Game::update(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		wrld->update();

		auto& registry = renderingEngine->getRegistry();

		systems::updateResourceProcessingSystem(registry, deltaTime, wrld->getHeightGenerator());

		daynight.update(deltaTime);
		auto sunDir = glm::normalize(daynight.getSunDirection());
		registry.replace<rendering::components::DirectionalLight>(sun, daynight.getSunColor(), sunDir);
		registry.replace<rendering::components::DirectionalLight>(starlight, daynight.getStarlight(), glm::vec3(0,1,0));

		auto& baseTf = registry.get<rendering::components::EulerComponentwiseTransform>(cameraBase);
		auto up = glm::cross(sunDir, glm::vec3(1,0,0));
		auto tf = glm::inverse(glm::lookAt(100.f * sunDir + baseTf.getTranslation(), glm::vec3(0) + baseTf.getTranslation(), up));
		// big shadow map shouldn't move with camera
		auto tf1 = glm::inverse(glm::lookAt(700.f * sunDir, glm::vec3(0), up));
		registry.replace<rendering::components::MatrixTransform>(shadowCamera, tf);
		registry.replace<rendering::components::MatrixTransform>(shadowCamera1, tf1);

		auto& cameraTf = registry.get<rendering::components::EulerComponentwiseTransform>(defaultCamera);
		float width = 1.2f * glm::length(cameraTf.getTranslation());
		width = std::min(width, 650.f);
		shadowCamParams->setWidth(width);


		float shadowMult = daynight.getShadows();
		registry.patch<rendering::components::CastShadow>(shadowCamera, [shadowMult](auto& castShadow) {
			castShadow.shadowMult = shadowMult;
		});
		registry.patch<rendering::components::CastShadow>(shadowCamera1, [shadowMult](auto& castShadow) {
			castShadow.shadowMult = shadowMult;
		});


		auto camPointer = selectedCamera == gui::CameraType::DEFAULT ? defaultCamera : freeFlightCamera;
		renderingEngine->setMainCamera(camPointer);

		selectChunks(registry, renderingEngine, wrld);

		//int a = 0;
		//if (glfwGetTime() >= time + 5.f && !added) {
		//	added = true;
		//	std::cout << "ADDING LIGHTS" << std::endl;
		//	for (auto c : wrld->getChunks()) {
		//		for (auto cell : c.second->getCells()) {
		//			auto* content = cell.second->getContent();
		//			if (content == nullptr) continue;

		//			a++;
		//			if (a % 10 != 0) continue;

		//			//glm::vec3 pos = glm::vec3(content->getTransform().getTransform()[3]);
		//			auto& registry = renderingEngine->getRegistry();
		//			auto light = registry.create();
		//			registry.emplace<rendering::components::MatrixTransform>(light, content->getTransform().getTransform());
		//			registry.emplace<rendering::components::PointLight>(light, glm::vec3(30), glm::vec3(0,5,0), glm::vec3(0,0,2));
		//		}
		//	}
		//}

		/*if (added) {
			for (auto entity : registry.view<rendering::components::PointLight, rendering::components::MatrixTransform>()) {
				auto& mt = registry.get<rendering::components::MatrixTransform>(entity);
				registry.replace<rendering::components::MatrixTransform>(entity, 
					mt.getTransform() * glm::rotate((float) deltaTime * 2.5f, glm::vec3(0, 1, 0)));
			}
		}*/
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
		gui::renderToolSelection(&selectedTool, &selectedBuilding, renderingEngine->getFramebufferHeight());

	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete wrld;
	}
}
