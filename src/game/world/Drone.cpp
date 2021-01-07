#include "Drone.hpp"

namespace game::world
{
	static auto* space = new rendering::bounding_geometry::Sphere::ObjectSpace();
	static auto boundingGeometry = std::make_shared<rendering::bounding_geometry::Sphere>(space);
	static rendering::model::MeshData droneMeshData = rendering::model::MeshData("test");
	static rendering::model::Mesh* droneMesh = nullptr;

	void Drone::spawnNewDrone(entt::registry& registry, const glm::vec3& position)
	{
		entt::entity droneEntity = registry.create();

		if (droneMesh == nullptr)
			droneMesh = new rendering::model::Mesh(droneMeshData, boundingGeometry);

		registry.emplace<rendering::components::MeshRenderer>(droneEntity, droneMesh);
		registry.emplace<rendering::components::CullingGeometry>(droneEntity, boundingGeometry);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(droneEntity, position, 0, 0, 0, glm::vec3(1.0f));
	}

	DroneGoal* MoveToGoal::destinationReached(entt::registry& registry, Drone* drone)
	{
		//Nothing to do here...
		return nullptr;
	}

	DroneGoal* HarvestGoal::destinationReached(entt::registry& registry, Drone* drone)
	{
		CellContent* content = destination->getContent();
		if (content == nullptr)
			return nullptr;

		entt::entity contentEntity = content->getEntity();

		// TODO: Harvest and pickup items.

		return nullptr;
	}

	DroneGoal* DeliveryGoal::destinationReached(entt::registry& registry, Drone* drone)
	{
		CellContent* content = destination->getContent();
		if (content == nullptr)
			return nullptr;

		entt::entity contentEntity = content->getEntity();

		// TODO: Drop off items.

		return nullptr;
	}
}
