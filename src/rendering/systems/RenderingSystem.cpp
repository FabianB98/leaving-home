#include "RenderingSystem.hpp"

namespace rendering::systems
{
	void updateLights(entt::registry& registry, rendering::shading::LightSupportingShader& shader)
	{
		// DIRECTIONAL LIGHT
		auto directional = registry.view<components::DirectionalLight, components::MatrixTransform>().front();
		if (registry.valid(directional)) {
			auto [light, transform] = registry.get<components::DirectionalLight, components::MatrixTransform>(directional);
			// transform the direction to world space (multiply direction with transform matrix)
			auto worldDir = transform.getTransform() * glm::vec4(light.direction, 0.f);
			shader.setUniformDirectionalLight("directionalLight", light.intensity, worldDir);
		}

		// POINT LIGHTS
		std::vector<glm::vec3> intensities;
		std::vector<glm::vec3> positions;
		auto lights = registry.view<components::PointLight, components::MatrixTransform>();
		for (auto entity : lights) {
			auto [light, transform] = registry.get<components::PointLight, components::MatrixTransform>(entity);

			intensities.push_back(light.intensity);
			// transform light position to world space
			positions.push_back(transform.getTransform() * glm::vec4(light.position, 1.f));
		}

		shader.setUniformPointLights("pointLights", intensities, positions);
	}

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
