#include "RenderingSystem.hpp"

namespace rendering::systems
{
	extern model::Mesh* pointLightVolume;
	extern model::Mesh* spotLightVolume;
	extern shading::Shader* phongShaderPoint;
	extern shading::Shader* phongShaderSpot;
	extern std::shared_ptr<bounding_geometry::BoundingGeometry> pointLightCulling;
	extern std::shared_ptr<bounding_geometry::BoundingGeometry> spotLightCulling;

	extern entt::observer transformObserver;

	extern std::set<model::Mesh*> transformChanged;
	extern std::unordered_map<model::Mesh*, std::pair<std::vector<glm::mat4>, std::vector<glm::mat3>>> meshTransforms;
	extern std::vector<std::pair<model::Mesh*, shading::Shader*>> meshShaders;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat4>> modelMatricesToRender;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat3>> normalMatricesToRender;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat4>> mvpMatricesToRender;

	extern components::CullingGeometry cullingRoot;

	extern glm::mat4 viewMatrix;
	extern glm::mat3 viewNormalMatrix;
	extern std::vector<float> shadowMultipliers;
	extern std::vector<glm::mat4> shadowMatrices;
	extern std::vector<glm::mat4> shadowWorldMatrices;

	extern std::vector<glm::vec3> directionalIntensities;
	extern std::vector<glm::vec3> directionalDirsWorld;
	extern std::vector<glm::vec3> directionalDirsView;

	std::unordered_map<shading::Shader*, int> shaderPriorities;
	std::unordered_map<model::Mesh*, std::vector<std::size_t>> instancesToRender;
	std::unordered_map<entt::entity, size_t> entityToTransformIndexMap;

	extern void changedMeshRenderer(entt::registry&, entt::entity);

	void updateDirectionalLights(entt::registry& registry, entt::entity shadowLight)
	{
		directionalIntensities.clear();
		directionalDirsWorld.clear();
		directionalDirsView.clear();
		int shadowLightIndex = 0;

		auto directional = registry.view<components::DirectionalLight, components::MatrixTransform>();
		for (auto entity : directional) {
			auto [light, transform] = registry.get<components::DirectionalLight, components::MatrixTransform>(entity);
			if (entity == shadowLight)
				shadowLightIndex = directionalIntensities.size();

			directionalIntensities.push_back(light.intensity);
			// transform the direction to world space and view space (multiply direction with transform matrix)
			// deferred shading uses view space, forward shading uses world space
			glm::vec4 worldDir = transform.getTransform() * glm::vec4(light.direction, 0.f);
			directionalDirsWorld.push_back(worldDir);
			directionalDirsView.push_back(viewMatrix * worldDir);
		}

		// move shading casting light to the first position in lists
		if (shadowLightIndex != 0) {
			std::swap(directionalIntensities[0], directionalIntensities[shadowLightIndex]);
			std::swap(directionalDirsWorld[0], directionalDirsWorld[shadowLightIndex]);
			std::swap(directionalDirsView[0], directionalDirsView[shadowLightIndex]);
		}
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
		auto& models = meshTransforms[pointLightVolume].first;
		auto& normals = meshTransforms[pointLightVolume].second;

		models.clear();
		normals.clear();

		auto group = registry.group<components::PointLight>(entt::get<components::MatrixTransform>);
		for (auto entity : group) {
			auto& pLight = group.get<components::PointLight>(entity);
			auto& transform = registry.get<components::MatrixTransform>(entity);
			auto relationship = registry.try_get<components::Relationship>(entity);

			glm::mat4& modelMatrix = relationship ? relationship->totalTransform : transform.getTransform();
			entityToTransformIndexMap[entity] = models.size();
			glm::vec4 lightPosWorld = modelMatrix * glm::vec4(pLight.position, 1.f);
			float radius = pLight.getRadius();

			// Calculate the model matrix without matrix multiplications
			glm::mat4 model = glm::scale(glm::vec3(radius));
			model[3] = lightPosWorld;

			glm::vec4 lightPos = viewMatrix * lightPosWorld;

			// fill the "normal" matrix with light info
			glm::mat3 lightInfo;
			lightInfo[0] = pLight.intensity;
			lightInfo[1] = lightPos;
			lightInfo[2] = pLight.attenuation;

			models.push_back(model);
			normals.push_back(lightInfo);
		}
	}

	void updateSpotLights(entt::registry& registry)
	{
		auto& models = meshTransforms[spotLightVolume].first;
		auto& normals = meshTransforms[spotLightVolume].second;

		models.clear();
		normals.clear();

		auto group = registry.group<components::SpotLight>(entt::get<components::MatrixTransform>);
		for (auto entity : group) {
			auto& sLight = group.get<components::SpotLight>(entity);
			auto& transform = registry.get<components::MatrixTransform>(entity);
			auto relationship = registry.try_get<components::Relationship>(entity);

			glm::mat4& modelMatrix = relationship ? relationship->totalTransform : transform.getTransform();
			entityToTransformIndexMap[entity] = models.size();
			
			glm::vec4 lightPosWorld = modelMatrix * glm::vec4(sLight.position, 1.f);
			glm::vec4 dirWorld = modelMatrix * glm::vec4(sLight.direction, 0.f);
			float radius = sLight.getRadius();
			float xyScale = radius * tanf(sLight.fov);

			glm::vec3 other;
			if (dirWorld.x == 0.f) other = glm::vec3(1, 0, 0);
			else other = glm::vec3(0, 1, 0);

			glm::mat4 model = glm::inverse(glm::lookAt(glm::vec3(lightPosWorld), 
				glm::vec3(lightPosWorld + dirWorld), glm::cross(glm::vec3(dirWorld), other)));
			model = model * glm::scale(glm::vec3(xyScale, xyScale, radius));

			glm::vec4 lightPos = viewMatrix * lightPosWorld;
			glm::vec3 packedDir = sLight.packDirection(viewMatrix * dirWorld);

			// fill the "normal" matrix with light info
			glm::mat3 lightInfo;
			lightInfo[0] = sLight.intensity;
			lightInfo[1] = lightPos;
			lightInfo[2] = packedDir;

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

	void performFrustumCulling(entt::registry& registry, rendering::components::Camera& camera, std::function<bool(model::Mesh*)> exclude)
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
				if (exclude(mesh)) continue;

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
		rendering::components::Camera& mainCamera, std::vector<entt::entity> shadowCameras,
		rendering::shading::Shader* defaultShader, bool overrideShaders)
	{
		viewMatrix = mainCamera.getViewMatrix();
		auto viewInv = glm::inverse(viewMatrix);
		viewNormalMatrix = glm::mat3(glm::transpose(viewInv));

		shadowMultipliers.clear();
		shadowMatrices.clear();
		shadowWorldMatrices.clear();

		for (unsigned int i = 0; i < shadowCameras.size(); ++i) {
			auto& shadowComp = registry.get<components::CastShadow>(shadowCameras[i]);
			shadowMultipliers.push_back(shadowComp.shadowMult);
			auto& shadowCamComp = registry.get<components::Camera>(shadowCameras[i]);
			shadowMatrices.push_back(shadowCamComp.getViewProjectionMatrix() * viewInv);
			shadowWorldMatrices.push_back(shadowCamComp.getViewProjectionMatrix());
		}

		// Get all entities whose transformation was changed and store that their transformation was changed.
		for (const auto entity : transformObserver) {
			changedMeshRenderer(registry, entity);
		}

		// Update the model and normal matrices of all meshes where ate lest one transformation was changed.
		if (transformChanged.size() != 0)
			updateMeshTransforms(registry);

		//if (pointLightObserver.size() != 0)
		updatePointLights(registry);
		updateSpotLights(registry);

		// get directional light that casts the shadow
		auto& shadowComp = registry.get<components::CastShadow>(shadowCameras[0]);
		updateDirectionalLights(registry, shadowComp.directionalLight);

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
	}

	void renderUpdateMVPs(entt::registry& registry, components::Camera& camera, std::function<bool(model::Mesh*)> exclude)
	{
		performFrustumCulling(registry, camera, exclude);
		calculateMVPs(camera);
	}
}