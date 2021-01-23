#include "Drone.hpp"

namespace game::world
{
	static auto* droneBoundingSpace = new rendering::bounding_geometry::Sphere::ObjectSpace();
	static auto droneBoundingGeometry = std::make_shared<rendering::bounding_geometry::Sphere>(droneBoundingSpace);
	static rendering::model::MeshData droneMeshData = rendering::model::MeshData("drone");
	static rendering::model::Mesh* droneMesh = nullptr;

	static auto* rotorBoundingSpace = new rendering::bounding_geometry::Sphere::ObjectSpace();
	static auto rotorBoundingGeometry = std::make_shared<rendering::bounding_geometry::Sphere>(rotorBoundingSpace);
	static rendering::model::MeshData rotorMeshData = rendering::model::MeshData("rotor");
	static rendering::model::Mesh* rotorMesh = nullptr;

	static auto* crateBoundingSpace = new rendering::bounding_geometry::Sphere::ObjectSpace();
	static auto crateBoundingGeometry = std::make_shared<rendering::bounding_geometry::Sphere>(crateBoundingSpace);
	static rendering::model::MeshData crateMeshData = rendering::model::MeshData("crate");
	static rendering::model::Mesh* crateMesh = nullptr;

	std::default_random_engine randomEngine;
	std::uniform_real_distribution<float> wobbleDistribution(0.5f, 1.0f);

	void Drone::inventoryUpdated(entt::registry& registry, entt::entity& entity, Inventory& inventory)
	{
		if (inventory.items.empty())
			registry.remove_if_exists<rendering::components::MeshRenderer>(crateEntity);
		else
			registry.emplace_or_replace<rendering::components::MeshRenderer>(crateEntity, crateMesh);
	}

	void Drone::spawnNewDrone(entt::registry& registry, const glm::vec3& position)
	{
		entt::entity droneEntity = registry.create();
		entt::entity rotor1Entity = registry.create();
		entt::entity rotor2Entity = registry.create();
		entt::entity rotor3Entity = registry.create();
		entt::entity crateEntity = registry.create();
		entt::entity spotLightEntity = registry.create();

		if (droneMesh == nullptr)
		{
			droneMesh = new rendering::model::Mesh(droneMeshData, droneBoundingGeometry);
			rotorMesh = new rendering::model::Mesh(rotorMeshData, rotorBoundingGeometry);
			crateMesh = new rendering::model::Mesh(crateMeshData, crateBoundingGeometry);

			auto& shadows = registry.ctx<rendering::systems::ShadowMapping>();
			shadows.castShadow.insert(std::make_pair(droneMesh, 0));
		}

		registry.emplace<Drone>(droneEntity, rotor1Entity, rotor2Entity, rotor3Entity, crateEntity, spotLightEntity, wobbleDistribution(randomEngine));
		registry.emplace<Inventory>(droneEntity);
		registry.emplace<rendering::components::MeshRenderer>(droneEntity, droneMesh);
		registry.emplace<rendering::components::CullingGeometry>(droneEntity, droneBoundingGeometry);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(droneEntity, position, 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<rendering::components::Relationship>(droneEntity);

		registry.emplace<rendering::components::MeshRenderer>(rotor1Entity, rotorMesh);
		registry.emplace<rendering::components::CullingGeometry>(rotor1Entity, rotorBoundingGeometry);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(rotor1Entity, glm::vec3(0.0f, 0.72f, -1.8f), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<rendering::components::Relationship>(rotor1Entity);

		registry.emplace<rendering::components::MeshRenderer>(rotor2Entity, rotorMesh);
		registry.emplace<rendering::components::CullingGeometry>(rotor2Entity, rotorBoundingGeometry);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(rotor2Entity, glm::vec3(-1.5f, 0.72f, 1.2f), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<rendering::components::Relationship>(rotor2Entity);

		registry.emplace<rendering::components::MeshRenderer>(rotor3Entity, rotorMesh);
		registry.emplace<rendering::components::CullingGeometry>(rotor3Entity, rotorBoundingGeometry);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(rotor3Entity, glm::vec3(1.5f, 0.72f, 1.2f), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<rendering::components::Relationship>(rotor3Entity);

		// MeshRenderer for the crate mesh is not yet added as the drone starts of with an empty inventory.
		registry.emplace<rendering::components::CullingGeometry>(crateEntity, crateBoundingGeometry);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(crateEntity, glm::vec3(0.0f), 0, 0, 0, glm::vec3(1.0f));
		registry.emplace<rendering::components::Relationship>(crateEntity);

		rendering::systems::relationship(registry, droneEntity, rotor1Entity);
		rendering::systems::relationship(registry, droneEntity, rotor2Entity);
		rendering::systems::relationship(registry, droneEntity, rotor3Entity);
		rendering::systems::relationship(registry, droneEntity, crateEntity);

		rendering::systems::cullingRelationship(registry, droneEntity, rotor1Entity);
		rendering::systems::cullingRelationship(registry, droneEntity, rotor2Entity);
		rendering::systems::cullingRelationship(registry, droneEntity, rotor3Entity);
		// Crate is not guaranteed to be fully included in the culling geometry of the drone. Therefore, it can't be a culling
		// child of the drone.

		registry.emplace<rendering::components::MatrixTransform>(spotLightEntity, glm::mat4(1));
		registry.emplace<rendering::components::Relationship>(spotLightEntity);
		registry.emplace<rendering::components::SpotLight>(spotLightEntity, glm::vec3(0), glm::vec3(0), glm::vec3(0, -1, 0), 3.1416f / 12.f);

		rendering::systems::relationship(registry, droneEntity, spotLightEntity);
	}
}
