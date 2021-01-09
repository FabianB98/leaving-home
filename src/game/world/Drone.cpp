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

		registry.emplace<Drone>(droneEntity);
		registry.emplace<rendering::components::MeshRenderer>(droneEntity, droneMesh);
		registry.emplace<rendering::components::CullingGeometry>(droneEntity, boundingGeometry);
		registry.emplace<rendering::components::EulerComponentwiseTransform>(droneEntity, position, 0, 0, 0, glm::vec3(1.0f));
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
