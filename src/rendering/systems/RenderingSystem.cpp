#include "RenderingSystem.hpp"

namespace rendering::systems
{
	entt::observer transformObserver;

	std::set<model::Mesh*> transformChanged;
	std::unordered_map<model::Mesh*, std::pair<std::vector<glm::mat4>, std::vector<glm::mat3>>> meshTransforms;
	std::unordered_map<entt::entity, size_t> entityToTransformIndexMap;
	std::vector<std::pair<model::Mesh*, shading::Shader*>> meshShaders;
	std::unordered_map<shading::Shader*, int> shaderPriorities;
	std::unordered_map<model::Mesh*, std::vector<glm::mat4>> modelMatricesToRender;
	std::unordered_map<model::Mesh*, std::vector<glm::mat3>> normalMatricesToRender;
	std::unordered_map<model::Mesh*, std::vector<glm::mat4>> mvpMatricesToRender;
	std::unordered_map<model::Mesh*, std::vector<std::size_t>> instancesToRender;

	components::CullingGeometry cullingRoot = components::CullingGeometry(std::make_shared<bounding_geometry::None>());

	void changedMeshRenderer(entt::registry& registry, entt::entity entity)
	{
		auto& meshRenderer = registry.get<components::MeshRenderer>(entity);
		transformChanged.insert(meshRenderer.getMesh());
	}

	void addedCullingGeometry(entt::registry& registry, entt::entity entity)
	{
		auto& cullingGeometry = registry.get<components::CullingGeometry>(entity);
		cullingGeometry.parent = entt::null;
		cullingRoot.children.insert(entity);
	}

	void removedCullingGeometry(entt::registry& registry, entt::entity entity)
	{
		components::CullingGeometry& deletedCullingGeometry = registry.get<components::CullingGeometry>(entity);

		components::CullingGeometry& parentCullingGeometry = deletedCullingGeometry.parent != entt::null
			? registry.get<components::CullingGeometry>(deletedCullingGeometry.parent) 
			: cullingRoot;

		parentCullingGeometry.children.erase(entity);
	}

	void cullingRelationship(entt::registry& registry, entt::entity parent, entt::entity child)
	{
		components::CullingGeometry& childCullingGeometry = registry.get<components::CullingGeometry>(child);

		components::CullingGeometry& oldParentCullingGeometry = childCullingGeometry.parent != entt::null
			? registry.get<components::CullingGeometry>(childCullingGeometry.parent)
			: cullingRoot;

		components::CullingGeometry& newParentCullingGeometry = parent != entt::null
			? registry.get<components::CullingGeometry>(parent)
			: cullingRoot;

		childCullingGeometry.parent = parent;
		oldParentCullingGeometry.children.erase(child);
		newParentCullingGeometry.children.insert(child);
	}

	void initRenderingSystem(entt::registry& registry)
	{
		registry.set<MeshShading>();
		registry.set<Picking>();

		registry.on_construct<components::MeshRenderer>().connect<&changedMeshRenderer>();
		registry.on_destroy<components::MeshRenderer>().connect<&changedMeshRenderer>();
		transformObserver.connect(registry, entt::collector.update<components::MatrixTransform>().where<components::MeshRenderer>());

		registry.on_construct<components::CullingGeometry>().connect<&addedCullingGeometry>();
		registry.on_destroy<components::CullingGeometry>().connect<&removedCullingGeometry>();
	}

	void updateLights(entt::registry& registry, rendering::shading::Shader& shader)
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

			auto& transform = registry.get<components::MatrixTransform>(entity);
			auto relationship = registry.try_get<components::Relationship>(entity);

			glm::mat4& modelMatrix = relationship ? relationship->totalTransform : transform.getTransform();
			entityToTransformIndexMap[entity] = meshTransforms[mesh].first.size();
			meshTransforms[mesh].first.push_back(modelMatrix);
			meshTransforms[mesh].second.push_back(glm::mat3(glm::transpose(glm::inverse(modelMatrix)))); 
		}


		// Calculate the maximum amount of instances of one mesh of meshes where at least one transformation was changed.
		size_t numInstances = 0;
		for (const auto& mesh : transformChanged)
			numInstances = std::max(numInstances, meshTransforms[mesh].first.size());
		return numInstances;
	}

	void activateShader(entt::registry& registry, rendering::components::Camera& camera, shading::Shader& shader, uint32_t pickingID)
	{
		shader.use();
		shader.setUniformFloat("time", (float)glfwGetTime());
		shader.setUniformInt("pick", pickingID);
		camera.applyViewProjection(shader);
		updateLights(registry, shader);
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

	void renderRenderingSystemTransforms(entt::registry& registry, 
		rendering::components::Camera& camera, rendering::shading::Shader* defaultShader, bool overrideShaders)
	{
		// Get all entities whose transformation was changed and store that their transformation was changed.
		for (const auto entity : transformObserver) {
			changedMeshRenderer(registry, entity);
		}

		// Update the model and normal matrices of all meshes where ate lest one transformation was changed.
		if (transformChanged.size() != 0)
			updateMeshTransforms(registry);

		transformObserver.clear();
		transformChanged.clear();

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

	void renderRenderingSystemForward(entt::registry& registry, rendering::components::Camera& camera, uint32_t pickingID)
	{
		// Render each mesh with the corresponding shader.
		shading::Shader* activeShader = NULL;
		for (const auto& meshShaderPairs : meshShaders)
		{
			auto* mesh = meshShaderPairs.first;
			const auto& meshModels = modelMatricesToRender[mesh];
			size_t numInstances = meshModels.size();

			if (numInstances > 0)
			{
				// switch shader if necessary
				auto* shader = meshShaderPairs.second;
				if (shader != activeShader || activeShader == NULL) {
					activeShader = shader;
					activateShader(registry, camera, *activeShader, pickingID);
				}
				
				const auto& meshNormals = normalMatricesToRender[mesh];
				const auto& meshMVPs = mvpMatricesToRender[mesh];

				// Render all instances of the mesh.
				mesh->renderInstanced(*activeShader, meshModels, meshNormals, meshMVPs);
			}
		}
	}

	void renderRenderingSystemPicking(entt::registry& registry, rendering::components::Camera& camera, shading::Shader* pickingShader)
	{
		auto& picking = registry.ctx<Picking>();
		if (picking.enabled.size() == 0) return;

		pickingShader->use();
		camera.applyViewProjection(*pickingShader);

		for (const auto& mesh : picking.enabled)
		{
			const auto& meshInstances = meshTransforms[mesh];
			const auto& meshModels = modelMatricesToRender[mesh];
			size_t numInstances = meshModels.size();

			if (numInstances > 0)
			{
				const auto& meshNormals = normalMatricesToRender[mesh];
				const auto& meshMVPs = mvpMatricesToRender[mesh];

				// Render all instances of the mesh.
				mesh->renderInstanced(*pickingShader, meshModels, meshNormals, meshMVPs);
			}
		}
	}

	void cleanUpRenderingSystem(entt::registry& registry)
	{
		registry.on_construct<components::MeshRenderer>().disconnect<&changedMeshRenderer>();
		registry.on_destroy<components::MeshRenderer>().disconnect<&changedMeshRenderer>();
		transformObserver.disconnect();

		registry.on_construct<components::CullingGeometry>().disconnect<&addedCullingGeometry>();
		registry.on_destroy<components::CullingGeometry>().disconnect<&removedCullingGeometry>();
	}
}
