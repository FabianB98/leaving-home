#include "Game.hpp"

#include "../rendering/components/Camera.hpp"
#include "../rendering/components/MeshRenderer.hpp"
#include "../rendering/components/Transform.hpp"
#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Lights.hpp"
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
		glm::mat4 transform = glm::translate(glm::vec3(0, -2, 0)) * glm::scale(glm::vec3(0.5));
		registry.emplace<rendering::components::Transform>(tree, transform);
	}

	void Game::input(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{

	}

	void Game::update(rendering::RenderingEngine* renderingEngine, double deltaTime)
	{
		auto& registry = renderingEngine->getRegistry();

		registry.patch<rendering::components::Transform>(tree, [](auto& transform) 
		{ 
			transform.setTransform(
				glm::rotate(rotation, glm::vec3(0, 1, 0)) *
				glm::translate(glm::vec3(0, -2, 0)) *
				glm::scale(glm::vec3(0.5))
			);
		});
	}

	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		ImGui::Begin("Test window");

		ImGui::SliderFloat("Tree rotation", &rotation, 0.f, 4.f);

		ImGui::End();
	}

	void Game::cleanUp(rendering::RenderingEngine* renderingEngine)
	{
		renderingEngine->getRegistry().clear();
		delete mesh;
	}
}
