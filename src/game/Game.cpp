#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/components/Lights.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glfw/glfw3.h>
#include <imgui.h>

namespace game
{
	rendering::model::Mesh* mesh;
	entt::entity tree;
	float rotation = 0;

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));

		auto& registry = renderingEngine->getRegistry();
		tree = registry.create();

		mesh = new rendering::model::Mesh("tree");
		registry.emplace<rendering::components::MeshRenderer>(tree, mesh);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(tree, glm::vec3(0, -1.6, 0), 0, 0, 0, glm::vec3(0.5));


		auto tree2 = registry.create();
		registry.emplace<rendering::components::MeshRenderer>(tree2, mesh);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(tree2, glm::vec3(2, -1.6, 0), 0, 0, 0, glm::vec3(0.4));


		auto sun = registry.create();
		registry.emplace<rendering::components::MatrixTransform>(sun, glm::mat4(1.f));
		registry.emplace<rendering::components::DirectionalLight>(sun, glm::vec3(2), glm::vec3(2, 1, 1));

		auto pLight = registry.create();
		registry.emplace<rendering::components::MatrixTransform>(pLight, glm::mat4(1.f));
		registry.emplace<rendering::components::PointLight>(tree, glm::vec3(50), glm::vec3(-1.5, -1.5, 1.5));
	}

	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{

	}

	void Game::update(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		auto& registry = renderingEngine->getRegistry();

		registry.patch<rendering::components::EulerComponentwiseTransform>(tree, [](auto& transform)
		{
			transform.setYaw(rotation);
		});
	}

	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		ImGui::Begin("Test window");

		ImGui::SliderFloat("Tree rotation", &rotation, 0.f, 6.3f);

		ImGui::End();
	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete mesh;
	}
}
