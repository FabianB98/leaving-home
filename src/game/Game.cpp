#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "../rendering/Skybox.hpp"
#include "components/FirstPersonRotateController.hpp"
#include "components/FreeFlyingMoveController.hpp"
#include "systems/MovementInputSystem.hpp"
#include "world/Chunk.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <time.h>
#include <math.h>

#include <chrono>

namespace game
{
	rendering::model::Mesh* mesh;
	rendering::model::Mesh* plane;
	rendering::model::Mesh* cube;
	/*entt::entity tree;
	float rotation = 0;*/

	world::Chunk* chunk1;
	world::Chunk* chunk2;
	world::Chunk* chunk3;
	world::Chunk* chunk4;
	world::Chunk* chunk5;
	world::Chunk* chunk6;
	world::Chunk* chunk7;

	entt::entity pLight;
	float red = 1, green = 1, blue = 1;

	float daytime = 0.f;
	float timeSpeed = 1.f;
	rendering::Skybox* skybox;

	double randomDouble()
	{
		return (double) rand() / (double) RAND_MAX;
	}

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		mesh = new rendering::model::Mesh("tree");
		plane = new rendering::model::Mesh("plane");
		cube = new rendering::model::Mesh("cube");

		skybox = new rendering::Skybox("skybox", "skybox");

		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));

		using namespace rendering::components;

		auto& registry = renderingEngine->getRegistry();

		auto start = std::chrono::high_resolution_clock::now();

		chunk1 = new world::Chunk(size_t(1337), 0, 0);
		entt::entity chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk1->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		chunk2 = new world::Chunk(size_t(1337), 0, 1);
		chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk2->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		chunk3 = new world::Chunk(size_t(1337), 0, 2);
		chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk3->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		chunk4 = new world::Chunk(size_t(1337), 1, 0);
		chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk4->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		chunk5 = new world::Chunk(size_t(1337), 1, 1);
		chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk5->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		chunk6 = new world::Chunk(size_t(1337), 1, 2);
		chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk6->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		chunk7 = new world::Chunk(size_t(1337), -1, 1);
		chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk7->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());

		auto finish = std::chrono::high_resolution_clock::now();
		std::cout << "Generated 7 chunks in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms" << std::endl;

		entt::entity camera = renderingEngine->getMainCamera();
		registry.emplace<components::FreeFlyingMoveController>(camera, 15.f);
		registry.emplace<components::FirstPersonRotateController>(camera, GLFW_MOUSE_BUTTON_RIGHT);
		registry.emplace<EulerComponentwiseTransform>(camera, glm::vec3(0, 5, 5), 0, 0, 0, glm::vec3(1.0f));


		auto entity = registry.create();
		registry.emplace<MeshRenderer>(entity, plane);
		registry.emplace<EulerComponentwiseTransform>(entity, glm::vec3(0), 0, 0, 0, glm::vec3(100));



		std::srand(time(NULL));

		float spacing = 5.f;
		int numTrees = 0;
		for (int i = -90; i < 100; i++) {
			for (int j = -90; j < 100; j++) {
				if (rand() % 2 == 0) continue;

				auto entity = registry.create();
				numTrees++;

				glm::vec3 pos(spacing * (double) i + 3.0*(randomDouble()-0.5), 0, spacing * (double) j + 3.0*(randomDouble() - 0.5));
				float yaw = 6.3f * randomDouble();
				float scale = .75f + .5f * randomDouble();

				registry.emplace<MeshRenderer>(entity, mesh);
				registry.emplace<EulerComponentwiseTransform>(entity, pos, yaw, 0, 0, glm::vec3(scale));
				//registry.emplace<components::FirstPersonRotateController>(entity, GLFW_MOUSE_BUTTON_MIDDLE);
			}
		}
		std::cout << numTrees << " trees generated" << std::endl;

		auto sun = registry.create();
		registry.emplace<MatrixTransform>(sun, glm::mat4(1.f));
		registry.emplace<DirectionalLight>(sun, glm::vec3(2), glm::vec3(2, 1, 1));

		pLight = registry.create();
		registry.emplace<EulerComponentwiseTransform>(pLight, glm::vec3(-1.5, 4, 1.5),0,0,0,glm::vec3(.5f));
		registry.emplace<MeshRenderer>(pLight, cube);
		registry.emplace<PointLight>(pLight, glm::vec3(50), glm::vec3(0));
//		registry.emplace<components::FirstPersonRotateController>(pLight, GLFW_MOUSE_BUTTON_MIDDLE);
		registry.emplace<components::FreeFlyingMoveController>(pLight, 
			GLFW_KEY_UP,
			GLFW_KEY_DOWN,
			GLFW_KEY_LEFT,
			GLFW_KEY_RIGHT,
			GLFW_KEY_E,
			GLFW_KEY_Q,
			5.f);
	}

	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		game::systems::updateMovementInputSystem(renderingEngine, deltaTime);
	}

	void Game::update(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		auto& registry = renderingEngine->getRegistry();

		registry.patch<rendering::components::PointLight>(pLight, [](auto& light)
		{
			light.intensity = 50.f * glm::vec3(red, green, blue);
		});

		daytime += timeSpeed * deltaTime;
		if (daytime >= M_PI) daytime -= 2 * M_PI;
	}

	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		skybox->use();
		skybox->getShader()->setUniformFloat("time", daytime);
		skybox->render(renderingEngine);


		ImGui::Begin("Test window");

		ImGui::SliderFloat("Red", &red, 0.f, 1.f);
		ImGui::SliderFloat("Green", &green, 0.f, 1.f);
		ImGui::SliderFloat("Blue", &blue, 0.f, 1.f);

		ImGui::Dummy(ImVec2(0, 10.f));

		ImGui::SliderFloat("Day-Night speed", &timeSpeed, 0.f, 1.f);

		ImGui::End();
	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete mesh;
		delete plane;
		delete cube;
		delete chunk1;
		delete chunk2;
		delete chunk3;
		delete chunk4;
		delete chunk5;
		delete chunk6;
		delete chunk7;
	}
}
