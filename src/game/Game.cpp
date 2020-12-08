#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/components/Relationship.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "../rendering/Skybox.hpp"
#include "components/FirstPersonRotateController.hpp"
#include "components/FreeFlyingMoveController.hpp"
#include "systems/MovementInputSystem.hpp"
#include "DayNightCycle.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

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

	double randomDouble()
	{
		return (double) rand() / (double) RAND_MAX;
	}

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		tree = new rendering::model::Mesh("tree");

		simple = new rendering::shading::Shader("simpleInstanced");
		waterShader = new rendering::shading::LightSupportingShader("waterInstanced", true);

		skybox = new rendering::Skybox("skybox", "skybox");

		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));

		using namespace rendering::components;

		auto& registry = renderingEngine->getRegistry();

		auto start = std::chrono::high_resolution_clock::now();

		wrld = new world::World(1337);
		wrld->generateChunk(0, 0);	// Center
		wrld->generateChunk(0, 1);	// Diag down right
		wrld->generateChunk(-1, 1);	// Diag down left
		wrld->generateChunk(-1, 0);	// Left
		wrld->generateChunk(0, -1); // Diag up left
		wrld->generateChunk(1, -1); // Diag up right
		wrld->generateChunk(1, 0);	// Right
		for (auto& chunk : wrld->getChunks()) {
			entt::entity chunkEntity = registry.create();
			registry.emplace<MeshRenderer>(chunkEntity, chunk.second->getMesh());
			registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 0, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());
		}

		entt::entity waterEntity = registry.create();
		registry.emplace<MeshRenderer>(waterEntity, wrld->getWaterMesh());
		registry.emplace<MatrixTransform>(waterEntity, EulerComponentwiseTransform(glm::vec3(0, 0, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		auto finish = std::chrono::high_resolution_clock::now();
		std::cout << "Generated 7 chunks in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms" << std::endl;

		entt::entity camera = renderingEngine->getMainCamera();
		registry.emplace<components::FreeFlyingMoveController>(camera, 15.f);
		registry.emplace<components::FirstPersonRotateController>(camera, GLFW_MOUSE_BUTTON_RIGHT);
		registry.emplace<EulerComponentwiseTransform>(camera, glm::vec3(0, 15, 5), 0, 0, 0, glm::vec3(1.0f));


		sun = registry.create();
		registry.emplace<MatrixTransform>(sun, glm::mat4(1.f));
		registry.emplace<DirectionalLight>(sun, glm::vec3(2), glm::vec3(2, 1, 1));




		auto& shading = registry.ctx<rendering::systems::MeshShading>();
		/*shading.shaders.insert(std::make_pair(wrld->getChunk(0, 0)->getMesh(), simple));
		shading.shaders.insert(std::make_pair(wrld->getChunk(-1, 1)->getMesh(), simple));
		shading.shaders.insert(std::make_pair(wrld->getChunk(0, -1)->getMesh(), simple));*/

		shading.shaders.insert(std::make_pair(wrld->getWaterMesh(), waterShader));


		shading.priorities.insert(std::make_pair(waterShader, 1));
	}

	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		game::systems::updateMovementInputSystem(renderingEngine, deltaTime);
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
