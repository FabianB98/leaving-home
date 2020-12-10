#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/components/Relationship.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "../rendering/Skybox.hpp"
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
	rendering::model::Mesh* tree;
	/*entt::entity tree;
	float rotation = 0;*/

	world::World* wrld;
	entt::entity sun;

	DayNightCycle daynight;
	rendering::Skybox* skybox;
	rendering::shading::Shader* simple;
	rendering::shading::Shader* waterShader;
	rendering::shading::Shader* terrainShader;

	double randomDouble()
	{
		return (double) rand() / (double) RAND_MAX;
	}

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		tree = new rendering::model::Mesh("tree");

		simple = new rendering::shading::Shader("simpleInstanced");
		waterShader = new rendering::shading::LightSupportingShader("waterInstanced", true);
		terrainShader = new rendering::shading::LightSupportingShader("terrainInstanced");

		skybox = new rendering::Skybox("skybox", "skybox");

		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));

		using namespace rendering::components;

		auto& registry = renderingEngine->getRegistry();
		auto& shading = registry.ctx<rendering::systems::MeshShading>();

		auto start = std::chrono::high_resolution_clock::now();

		wrld = new world::World(1337, registry, terrainShader, waterShader);
		for (int column = -5; column < 5; column++)
			for (int row = -5; row < 5; row++)
				wrld->generateChunk(column, row);

		auto finish = std::chrono::high_resolution_clock::now();
		std::cout << "Generated 100 chunks in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms" << std::endl;

		entt::entity cameraBase = registry.create();
		registry.emplace<EulerComponentwiseTransform>(cameraBase, glm::vec3(0, 0, 0), 0, glm::radians(-40.0f), 0, glm::vec3(1.0f));
		registry.emplace<components::HeightConstrainedMoveController>(cameraBase, GLFW_MOUSE_BUTTON_RIGHT, 8.0f);
		registry.emplace<components::FirstPersonRotateController>(cameraBase, GLFW_MOUSE_BUTTON_MIDDLE, 0.2f, glm::radians(-90.0f), glm::radians(-10.0f));
		registry.emplace<rendering::components::Relationship>(cameraBase);

		entt::entity camera = renderingEngine->getMainCamera();
		registry.emplace<EulerComponentwiseTransform>(camera, glm::vec3(0, 0, 100), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<components::AxisConstrainedMoveController>(camera, glm::vec3(0, 0, 1), 1000.0f, 50.0f, 500.0f);
		registry.emplace<rendering::components::Relationship>(camera);

		rendering::systems::relationship(registry, cameraBase, camera);

		sun = registry.create();
		registry.emplace<MatrixTransform>(sun, glm::mat4(1.f));
		registry.emplace<DirectionalLight>(sun, glm::vec3(1), glm::vec3(2, 1, 1));
	}

	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		game::systems::updateMovementInputSystem(renderingEngine, deltaTime, wrld->getHeightGenerator());
	}

	void Game::update(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		auto& registry = renderingEngine->getRegistry();

		daynight.update(deltaTime);
		registry.replace<rendering::components::DirectionalLight>(sun, daynight.getSunColor(), daynight.getSunDirection());
	}

	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		float time = daynight.getTime();

		skybox->use();
		skybox->getShader()->setUniformFloat("time", time);
		skybox->getShader()->setUniformVec3("sunColor", daynight.getSunColor());
		skybox->render(renderingEngine);


		ImGui::Begin("Test window");

		/*ImGui::SliderFloat("Red", &red, 0.f, 1.f);
		ImGui::SliderFloat("Green", &green, 0.f, 1.f);
		ImGui::SliderFloat("Blue", &blue, 0.f, 1.f);*/

		ImGui::Dummy(ImVec2(0, 10.f));

		ImGui::SliderFloat("Day-Night speed", &daynight.speed, -1.f, 1.f);

		ImGui::End();
	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete tree;
		delete wrld;
	}
}
