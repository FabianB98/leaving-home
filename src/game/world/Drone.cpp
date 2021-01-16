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

	void Drone::spawnNewDrone(entt::registry& registry, const glm::vec3& position)
	{
		entt::entity droneEntity = registry.create();
		entt::entity rotor1Entity = registry.create();
		entt::entity rotor2Entity = registry.create();
		entt::entity rotor3Entity = registry.create();
		entt::entity crateEntity = registry.create();

		if (droneMesh == nullptr)
		{
			droneMesh = new rendering::model::Mesh(droneMeshData, droneBoundingGeometry);
			rotorMesh = new rendering::model::Mesh(rotorMeshData, rotorBoundingGeometry);
			crateMesh = new rendering::model::Mesh(crateMeshData, crateBoundingGeometry);
		}

		registry.emplace<Drone>(droneEntity, rotor1Entity, rotor2Entity, rotor3Entity, crateEntity);
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

		registry.emplace<rendering::components::MeshRenderer>(crateEntity, crateMesh);
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
	}

	DroneGoal* MoveToGoal::destinationReached(entt::registry& registry, Drone& drone)
	{
		//Nothing to do here...
		return nullptr;
	}

	DroneGoal* HarvestGoal::destinationReached(entt::registry& registry, Drone& drone)
	{
		CellContent* content = destination->getContent();
		if (content == nullptr)
			return nullptr;

		entt::entity contentEntity = content->getEntity();
		Inventory* inventory = registry.try_get<Inventory>(contentEntity);
		if (inventory == nullptr)
			return nullptr;

		std::vector<std::shared_ptr<IItem>> itemsToRemoveFromInventory;
		for (std::shared_ptr<IItem> item : inventory->items)
			if (item->getHarvestable()->getFromEntity(registry, contentEntity) != nullptr)
			{
				drone.inventory.addItem(item);
				itemsToRemoveFromInventory.push_back(item);
			}

		for (std::shared_ptr<IItem> item : itemsToRemoveFromInventory)
			inventory->items.erase(item);

		return nullptr;
	}

	DroneGoal* PickupAndDeliveryGoal::destinationReached(entt::registry& registry, Drone& drone)
	{
		CellContent* content = destination->getContent();
		if (content == nullptr)
			return nullptr;

		entt::entity contentEntity = content->getEntity();
		Inventory* inventory = registry.try_get<Inventory>(contentEntity);
		if (inventory == nullptr)
			return nullptr;

		std::shared_ptr<IItem> item = inventory->getItem(itemType);
		if (item == nullptr)
			return nullptr;

		drone.inventory.addItem(item->split(amount));
		if (item->amount == 0.0f)
			inventory->items.erase(item);

		if (deliveryDestination == nullptr)
			return nullptr;
		else
			return new DeliveryGoal(deliveryDestination);
	}

	DroneGoal* DeliveryGoal::destinationReached(entt::registry& registry, Drone& drone)
	{
		CellContent* content = destination->getContent();
		if (content == nullptr)
			return nullptr;

		entt::entity contentEntity = content->getEntity();
		Inventory* inventory = registry.try_get<Inventory>(contentEntity);
		if (inventory == nullptr)
			return nullptr;

		std::vector<std::shared_ptr<IItem>> itemsToRemoveFromInventory;
		for (std::shared_ptr<IItem> item : drone.inventory.items)
		{
			bool consumesItem = item->getConsumes()->getFromEntity(registry, contentEntity) != nullptr;
			bool storesItem = item->getStores()->getFromEntity(registry, contentEntity) != nullptr;

			if (consumesItem || storesItem)
			{
				inventory->addItem(item);
				itemsToRemoveFromInventory.push_back(item);
			}
		}

		for (std::shared_ptr<IItem> item : itemsToRemoveFromInventory)
			drone.inventory.items.erase(item);

		return nullptr;
	}
}
