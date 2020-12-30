#include "RenderingSystem.hpp"

namespace rendering::systems
{
	extern model::Mesh* lightVolume;
	extern shading::Shader* phongShader;
	extern std::shared_ptr<bounding_geometry::BoundingGeometry> lightCulling;

	extern entt::observer transformObserver;
	extern entt::observer pointLightObserver;

	extern std::set<model::Mesh*> transformChanged;
	extern std::unordered_map<model::Mesh*, std::pair<std::vector<glm::mat4>, std::vector<glm::mat3>>> meshTransforms;
	extern std::vector<std::pair<model::Mesh*, shading::Shader*>> meshShaders;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat4>> modelMatricesToRender;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat3>> normalMatricesToRender;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat4>> mvpMatricesToRender;

	extern components::CullingGeometry cullingRoot;

	extern glm::mat4 viewMatrix;
	extern glm::mat3 viewNormalMatrix;

	std::unordered_map<shading::Shader*, int> shaderPriorities;
	std::unordered_map<model::Mesh*, std::vector<std::size_t>> instancesToRender;
	std::unordered_map<entt::entity, size_t> entityToTransformIndexMap;

	extern void changedMeshRenderer(entt::registry&, entt::entity);

	void updateDirectionalLights(entt::registry& registry, rendering::shading::Shader& shader)
	{
		std::vector<glm::vec3> intensities;
		std::vector<glm::vec3> directionsWorld;
		std::vector<glm::vec3> directionsView;
		auto directional = registry.view<components::DirectionalLight, components::MatrixTransform>();
		for (auto entity : directional) {
			auto [light, transform] = registry.get<components::DirectionalLight, components::MatrixTransform>(entity);

			intensities.push_back(light.intensity);
			// transform the direction to world space (multiply direction with transform matrix)
			glm::vec4 worldDir = transform.getTransform() * glm::vec4(light.direction, 0.f);
			directionsWorld.push_back(worldDir);
			directionsView.push_back(viewMatrix * worldDir);
		}

		shader.setUniformDirectionalLights("directionalLights", intensities, directionsWorld, directionsView);
	}

	void updateMeshTransforms(entt::registry& registry)
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

			auto& transform = registry.get<components::MatrixTransform>(entity);
			auto relationship = registry.try_get<components::Relationship>(entity);

			glm::mat4& modelMatrix = relationship ? relationship->totalTransform : transform.getTransform();
			entityToTransformIndexMap[entity] = meshTransforms[mesh].first.size();
			meshTransforms[mesh].first.push_back(modelMatrix);
			meshTransforms[mesh].second.push_back(glm::mat3(glm::transpose(glm::inverse(modelMatrix))));
		}
	}

	void updatePointLights(entt::registry& registry)
	{
		auto& models = meshTransforms[lightVolume].first;
		auto& normals = meshTransforms[lightVolume].second;

		models.clear();
		normals.clear();

		auto group = registry.group<components::PointLight>(entt::get<components::MatrixTransform>);
		for (auto entity : group) {
			auto& pLight = group.get<components::PointLight>(entity);
			auto& transform = registry.get<components::MatrixTransform>(entity);
			auto relationship = registry.try_get<components::Relationship>(entity);

			glm::mat4& modelMatrix = relationship ? relationship->totalTransform : transform.getTransform();
			entityToTransformIndexMap[entity] = models.size();
			glm::vec4 lightPos = viewMatrix * modelMatrix * glm::vec4(pLight.position, 1.f);
			float radius = pLight.getRadius();

			// Calculate the model matrix without matrix multiplications
			glm::mat4 model = glm::scale(glm::vec3(radius));
			model[3] = lightPos;

			// fill the "normal" matrix with light info
			glm::mat3 lightInfo;
			lightInfo[0] = pLight.intensity;
			lightInfo[1] = lightPos;
			lightInfo[2] = pLight.attenuation;

			models.push_back(model);
			normals.push_back(lightInfo);
		}
	}

	void calculateMVPs(rendering::components::Camera& camera)
	{
		glm::mat4& viewProjection = camera.getViewProjectionMatrix();
		for (const auto& meshShaderPairs : meshShaders)
		{
			auto* mesh = meshShaderPairs.first;
			const auto& meshInstances = meshTransforms[mesh];
			// Calculate the model view projection matrix of each instance of the mesh.
			const auto& instances = instancesToRender[mesh];
			int numInstances = instances.size();

			auto& meshModels = modelMatricesToRender[mesh];
			auto& meshNormals = normalMatricesToRender[mesh];
			auto& meshMVPs = mvpMatricesToRender[mesh];

			meshModels.resize(numInstances);
			meshNormals.resize(numInstances);
			meshMVPs.resize(numInstances);

			// Parallel mvp calculation (only calculate MVPs for instances which are not culled).
			std::vector<int> a(numInstances);
			std::iota(std::begin(a), std::end(a), 0);
			std::for_each(std::execution::par_unseq, std::begin(a), std::end(a), [&](int i) {
				size_t instance = instances[i];

				meshModels[i] = meshInstances.first[instance];
				meshNormals[i] = meshInstances.second[instance];
				meshMVPs[i] = viewProjection * meshInstances.first[instance];
				});
		}
	}

	void performFrustumCulling(entt::registry& registry, rendering::components::Camera& camera)
	{
		std::queue<entt::entity> entitiesToCheck;
		for (auto& entity : cullingRoot.children)
			entitiesToCheck.push(entity);

		glm::mat4& viewProjection = camera.getViewProjectionMatrix();
		const std::array<glm::vec4, 6>& cameraFrustum = camera.getClippingPlanes();

		instancesToRender.clear();
		for (auto& meshModelMatrices : meshTransforms)
			instancesToRender.insert(std::make_pair(meshModelMatrices.first, std::vector<size_t>()));

		while (!entitiesToCheck.empty())
		{
			entt::entity& entity = entitiesToCheck.front();
			entitiesToCheck.pop();

			components::MeshRenderer* meshRenderer = registry.try_get<components::MeshRenderer>(entity);
			model::Mesh* mesh;
			size_t index;
			glm::mat4 modelMatrix;
			if (meshRenderer != nullptr)
			{
				mesh = meshRenderer->getMesh();
				index = entityToTransformIndexMap[entity];
				modelMatrix = meshTransforms[mesh].first[index];
			}
			else
			{
				mesh = nullptr;
				index = 0;
				modelMatrix = glm::mat4(1.0f);
			}

			components::CullingGeometry& cullingGeometry = registry.get<components::CullingGeometry>(entity);
			if (cullingGeometry.boundingGeometry->isInCameraFrustum(cameraFrustum, modelMatrix))
			{
				if (meshRenderer != nullptr)
					instancesToRender[mesh].push_back(index);

				for (auto& child : cullingGeometry.children)
					entitiesToCheck.push(child);
			}
		}
	}

	void renderUpdateTransforms(entt::registry& registry,
		rendering::components::Camera& camera, rendering::shading::Shader* defaultShader, bool overrideShaders)
	{
		viewMatrix = camera.getViewMatrix();
		viewNormalMatrix = glm::mat3(glm::transpose(glm::inverse(viewMatrix)));

		// Get all entities whose transformation was changed and store that their transformation was changed.
		for (const auto entity : transformObserver) {
			changedMeshRenderer(registry, entity);
		}

		// Update the model and normal matrices of all meshes where ate lest one transformation was changed.
		if (transformChanged.size() != 0)
			updateMeshTransforms(registry);

		if (pointLightObserver.size() != 0)
			updatePointLights(registry);

		transformObserver.clear();
		transformChanged.clear();
		pointLightObserver.clear();

		// Fill meshShaders with all meshes from meshTransforms and their shaders
		meshShaders.clear();
		auto& shading = registry.ctx<MeshShading>();
		for (const auto& meshInstances : meshTransforms)
		{
			auto* mesh = meshInstances.first;
			auto& it = shading.shaders.find(mesh);
			auto* shader = (it == shading.shaders.end() || overrideShaders) ? defaultShader : it->second;
			meshShaders.push_back(std::make_pair(mesh, shader));
		}

		// Fill shader priorities with priorities from shading and add unknown shaders as priority 0
		shaderPriorities.clear();
		for (const auto& meshShader : meshShaders) {
			auto it = shading.priorities.find(meshShader.second);
			int priority = it != shading.priorities.end() ? it->second : 0;
			shaderPriorities.insert(std::make_pair(meshShader.second, priority));
		}

		// Sort meshShaders to group meshes with the same shaders
		// TODO: maybe use programID instead of pointer value?
		std::sort(meshShaders.begin(), meshShaders.end(), [](auto& ms1, auto& ms2) {
			int p1 = shaderPriorities[ms1.second];
			int p2 = shaderPriorities[ms2.second];

			return p1 != p2 ? p1 < p2 : std::less<shading::Shader*>()(ms1.second, ms2.second);
			});

		// update MVPs
		performFrustumCulling(registry, camera);
		calculateMVPs(camera);
	}
}