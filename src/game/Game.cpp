#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "components/FirstPersonRotateController.hpp"
#include "components/FreeFlyingMoveController.hpp"
#include "systems/MovementInputSystem.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <time.h>

namespace game
{
	rendering::model::Mesh* mesh;
	rendering::model::Mesh* plane;
	rendering::model::Mesh* cube;
	/*entt::entity tree;
	float rotation = 0;*/

	entt::entity pLight;
	float red = 1, green = 1, blue = 1;

	double randomDouble()
	{
		return (double) rand() / (double) RAND_MAX;
	}

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		mesh = new rendering::model::Mesh("tree");
		plane = new rendering::model::Mesh("plane");
		cube = new rendering::model::Mesh("cube");
		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));

		auto& registry = renderingEngine->getRegistry();

		entt::entity camera = renderingEngine->getMainCamera();
		registry.emplace<game::components::FreeFlyingMoveController>(camera);
		registry.emplace<game::components::FirstPersonRotateController>(camera, GLFW_MOUSE_BUTTON_RIGHT);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(camera, glm::vec3(0, 5, 5), 0, 0, 0, glm::vec3(1.0f));



		auto entity = registry.create();
		registry.emplace<rendering::components::MeshRenderer>(entity, plane);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(entity, glm::vec3(0), 0, 0, 0, glm::vec3(100));



		std::srand(time(NULL));

		float spacing = 5.f;
		for (int i = -19; i < 20; i++) {
			for (int j = -19; j < 20; j++) {
				if (rand() % 2 == 0) continue;

				auto entity = registry.create();

				glm::vec3 pos(spacing * (double) i + 3.0*(randomDouble()-0.5), 0, spacing * (double) j + 3.0*(randomDouble() - 0.5));
				float yaw = 6.3f * randomDouble();
				float scale = .75f + .5f * randomDouble();

				registry.emplace<rendering::components::MeshRenderer>(entity, mesh);
				registry.emplace<rendering::components::EulerComponentwiseTransform>(entity, pos, yaw, 0, 0, glm::vec3(scale));
			}
		}


		auto sun = registry.create();
		registry.emplace<rendering::components::MatrixTransform>(sun, glm::mat4(1.f));
		registry.emplace<rendering::components::DirectionalLight>(sun, glm::vec3(2), glm::vec3(2, 1, 1));

		pLight = registry.create();
		registry.emplace<rendering::components::EulerComponentwiseTransform>(pLight, glm::vec3(-1.5, 2, 1.5),0,0,0,glm::vec3(.5f));
		registry.emplace<rendering::components::MeshRenderer>(pLight, cube);
		registry.emplace<rendering::components::PointLight>(pLight, glm::vec3(50), glm::vec3(0));
		registry.emplace<game::components::FirstPersonRotateController>(pLight, GLFW_MOUSE_BUTTON_MIDDLE);
		registry.emplace<game::components::FreeFlyingMoveController>(pLight, 
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
	}

	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		ImGui::Begin("Test window");

		ImGui::SliderFloat("Red", &red, 0.f, 1.f);
		ImGui::SliderFloat("Green", &green, 0.f, 1.f);
		ImGui::SliderFloat("Blue", &blue, 0.f, 1.f);

		ImGui::End();
	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete mesh;
	}
}
