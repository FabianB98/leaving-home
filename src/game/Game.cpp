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

namespace game
{
	rendering::model::Mesh* mesh;
	rendering::model::Mesh* plane;
	rendering::model::Mesh* cube;
	/*entt::entity tree;
	float rotation = 0;*/

	world::Chunk* chunk;

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



		chunk = new world::Chunk(glm::vec2(0, 0));
		entt::entity chunkEntity = registry.create();
		registry.emplace<MeshRenderer>(chunkEntity, chunk->getMesh());
		registry.emplace<MatrixTransform>(chunkEntity, EulerComponentwiseTransform(glm::vec3(0, 20, 0), 0, 0, 0, glm::vec3(1)).toTransformationMatrix());



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
		for (int i = -190; i < 200; i++) {
			for (int j = -190; j < 200; j++) {
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
		delete chunk;
	}
}
