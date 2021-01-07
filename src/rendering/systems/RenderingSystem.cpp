#include "RenderingSystem.hpp"

namespace rendering::systems
{
	// Resources used for point lights and light volumes
	model::Mesh* lightVolume;
	shading::Shader* phongShader;
	std::shared_ptr<bounding_geometry::BoundingGeometry> lightCulling;

	// Handles additional uniforms during shader changes
	ShaderManager* shaderManager;

	// Observer for changed transform and light components
	entt::observer transformObserver;
	entt::observer pointLightObserver;

	// Collections for mesh rendering
	std::set<model::Mesh*> transformChanged;
	std::unordered_map<model::Mesh*, std::pair<std::vector<glm::mat4>, std::vector<glm::mat3>>> meshTransforms;
	std::vector<std::pair<model::Mesh*, shading::Shader*>> meshShaders;
	// Additional collections for culling
	std::unordered_map<model::Mesh*, std::vector<glm::mat4>> modelMatricesToRender;
	std::unordered_map<model::Mesh*, std::vector<glm::mat3>> normalMatricesToRender;
	std::unordered_map<model::Mesh*, std::vector<glm::mat4>> mvpMatricesToRender;

	std::vector<glm::vec3> directionalIntensities;
	std::vector<glm::vec3> directionalDirsWorld;
	std::vector<glm::vec3> directionalDirsView;

	glm::mat4 viewMatrix;
	glm::mat3 viewNormalMatrix;
	std::vector<float> shadowMultipliers;
	std::vector<glm::mat4> shadowMatrices;
	std::vector<glm::mat4> shadowWorldMatrices;

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

	void removedCullingGeometry(entt::registry& registry, entt::entity entity)
	{
		components::CullingGeometry& deletedCullingGeometry = registry.get<components::CullingGeometry>(entity);

		components::CullingGeometry& parentCullingGeometry = deletedCullingGeometry.parent != entt::null
			? registry.get<components::CullingGeometry>(deletedCullingGeometry.parent) 
			: cullingRoot;

		parentCullingGeometry.children.erase(entity);
	}

	void createdPointLight(entt::registry& registry, entt::entity entity)
	{
		registry.emplace_or_replace<components::CullingGeometry>(entity, lightCulling);
		registry.emplace_or_replace<components::MeshRenderer>(entity, lightVolume);
	}

	void destroyedPointLight(entt::registry& registry, entt::entity entity)
	{
		registry.remove_if_exists<components::CullingGeometry>(entity);
		registry.remove_if_exists<components::MeshRenderer>(entity);
	}

	void initRenderingSystem(entt::registry& registry, ShaderManager* manager)
	{
		shaderManager = manager;

		lightVolume = new model::Mesh("lightVolume");
		phongShader = new shading::Shader("deferred/phong");
		lightCulling = std::make_shared<rendering::bounding_geometry::Sphere>(glm::vec3(0.f), 1.f,
			new rendering::bounding_geometry::Sphere::ObjectSpace);

		registry.set<MeshShading>();
		registry.set<Picking>();
		registry.set<ShadowMapping>();

		// default light volumes are always rendered with the default phong shader
		registry.ctx<MeshShading>().shaders.insert(std::make_pair(lightVolume, phongShader));

		registry.on_construct<components::MeshRenderer>().connect<&changedMeshRenderer>();
		registry.on_destroy<components::MeshRenderer>().connect<&changedMeshRenderer>();
		transformObserver.connect(registry, entt::collector.update<components::MatrixTransform>().where<components::MeshRenderer>());

		registry.on_construct<components::PointLight>().connect<&createdPointLight>();
		registry.on_destroy<components::PointLight>().connect<&destroyedPointLight>();

		pointLightObserver.connect(registry, 
			entt::collector.update<components::PointLight>()
			.group<components::PointLight>()
			.update<components::MatrixTransform>()
			.where<components::PointLight>());
		entt::collector.update<components::PointLight>();

		registry.on_construct<components::CullingGeometry>().connect<&addedCullingGeometry>();
		registry.on_destroy<components::CullingGeometry>().connect<&removedCullingGeometry>();
	}

	void cleanUpRenderingSystem(entt::registry& registry)
	{
		delete lightVolume;
		delete phongShader;

		registry.on_construct<components::MeshRenderer>().disconnect<&changedMeshRenderer>();
		registry.on_destroy<components::MeshRenderer>().disconnect<&changedMeshRenderer>();
		transformObserver.disconnect();

		registry.on_construct<components::PointLight>().disconnect<&createdPointLight>();
		registry.on_destroy<components::PointLight>().disconnect<&destroyedPointLight>();
		pointLightObserver.disconnect();

		registry.on_construct<components::CullingGeometry>().disconnect<&addedCullingGeometry>();
		registry.on_destroy<components::CullingGeometry>().disconnect<&removedCullingGeometry>();
	}
}
