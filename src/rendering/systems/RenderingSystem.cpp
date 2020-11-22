#include "RenderingSystem.hpp"

namespace rendering::systems
{
	void renderRenderingSystem(entt::registry& registry, 
		rendering::components::Camera& camera, rendering::shading::Shader& shader)
	{
		auto meshes = registry.view<components::MeshRenderer, components::MatrixTransform>();
		for (auto entity : meshes) {
			auto [meshRenderer, transform] = registry.get<components::MeshRenderer, components::MatrixTransform>(entity);

			meshRenderer.render(shader, transform.getTransform(), camera.getViewProjectionMatrix());
		}
	}
}
