#include "RenderingSystem.hpp"

namespace rendering::systems
{
	entt::observer transformObserver;

	std::set<model::Mesh*> transformChanged;
	std::unordered_map<model::Mesh*, std::pair<std::vector<glm::mat4>, std::vector<glm::mat3>>> meshTransforms;
	std::vector<glm::mat4> mvps;

	void changedMeshRenderer(entt::registry& registry, entt::entity entity)
	{
		auto& meshRenderer = registry.get<components::MeshRenderer>(entity);
		transformChanged.insert(meshRenderer.getMesh());
	}

	void initRenderingSystem(entt::registry& registry)
	{
		registry.on_construct<components::MeshRenderer>().connect<&changedMeshRenderer>();
		registry.on_destroy<components::MeshRenderer>().connect<&changedMeshRenderer>();
		transformObserver.connect(registry, entt::collector.update<components::MatrixTransform>().where<components::MeshRenderer>());
	}

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

	size_t updateMeshTransforms(entt::registry& registry)
	{
		// Clear the stored model and normal matrices of all meshes where at least one transformation was changed.
		for (const auto& mesh : transformChanged) 
		{
			meshTransforms[mesh].first.clear();
			meshTransforms[mesh].second.clear();
		}

		auto group = registry.group<components::MeshRenderer>(entt::get<components::MatrixTransform>);
		for (auto entity : group) {
			auto& renderer = group.get<components::MeshRenderer>(entity);
			model::Mesh* mesh = renderer.getMesh();
			if (transformChanged.find(mesh) == transformChanged.end()) continue;

			auto& transform = group.get<components::MatrixTransform>(entity);
			auto relationship = registry.try_get<components::Relationship>(entity);

			glm::mat4 modelMatrix = relationship ? relationship->totalTransform : transform.getTransform();
			meshTransforms[mesh].first.push_back(modelMatrix);
			meshTransforms[mesh].second.push_back(glm::mat3(glm::transpose(glm::inverse(modelMatrix))));
		}


		// Calculate the maximum amount of instances of one mesh of meshes where at least one transformation was changed.
		size_t numInstances = 0;
		for (const auto& mesh : transformChanged)
			numInstances = std::max(numInstances, meshTransforms[mesh].first.size());
		return numInstances;
	}

	void renderRenderingSystem(entt::registry& registry, 
		rendering::components::Camera& camera, rendering::shading::Shader& shader)
	{
		// Get all entities whose transformation was changed and store that their transformation was changed.
		for (const auto entity : transformObserver) {
			changedMeshRenderer(registry, entity);
		}

		// Update the model and normal matrices of all meshes where ate lest one transformation was changed.
		if (transformChanged.size() != 0)
		{
			size_t maxInstancesPerMesh = updateMeshTransforms(registry);
			if (maxInstancesPerMesh > mvps.size())
				mvps.resize(maxInstancesPerMesh);
		}

		transformObserver.clear();
		transformChanged.clear();

		// Render each mesh.
		glm::mat4& viewProjection = camera.getViewProjectionMatrix();
		for (const auto& meshInstances : meshTransforms)
		{
			// Calculate the model view projection matrix of each instance of the mesh.
			int numInstances = meshInstances.second.first.size();

			// parallel mvp calculation
			std::vector<int> a(numInstances);
			std::iota(std::begin(a), std::end(a), 0);
			std::for_each(std::execution::par_unseq, std::begin(a), std::end(a), [&](int i) {
				mvps[i] = viewProjection * meshInstances.second.first[i];
			});

			// for reference: old mvp calculation
			/*for (int i = 0; i < numInstances; i++)
				mvps[i] = viewProjection * meshInstances.second.first[i];*/

			// Render all instances of the mesh.
			const std::vector<glm::mat4>& const modelMatrices = meshInstances.second.first;
			const std::vector<glm::mat3>& const normalMatrices = meshInstances.second.second;
			meshInstances.first->renderInstanced(shader, modelMatrices, normalMatrices, mvps);
		}
	}

	void cleanUpRenderingSystem(entt::registry& registry)
	{
		registry.on_construct<components::MeshRenderer>().disconnect<&changedMeshRenderer>();
		registry.on_destroy<components::MeshRenderer>().disconnect<&changedMeshRenderer>();
		transformObserver.disconnect();
	}
}
