#pragma once

#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../rendering/bounding_geometry/Sphere.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "Chunk.hpp"
#include "Item.hpp"

namespace game::world
{
	struct DroneGoal;

	struct Drone
	{
		DroneGoal* goal{ nullptr };
		Inventory inventory;

		static void spawnNewDrone(entt::registry& registry, const glm::vec3& position);
	};

	struct DroneGoal
	{
		Cell* destination;

		DroneGoal(Cell* _destination) : destination(_destination) {}

		virtual DroneGoal* destinationReached(entt::registry& registry, Drone& drone) = 0;
	};

	struct MoveToGoal : public DroneGoal
	{
		MoveToGoal(Cell* _destination) : DroneGoal(_destination) {}

		DroneGoal* destinationReached(entt::registry& registry, Drone& drone);
	};

	struct HarvestGoal : public DroneGoal
	{
		HarvestGoal(Cell* _destination) : DroneGoal(_destination) {}

		DroneGoal* destinationReached(entt::registry& registry, Drone& drone);
	};

	template <class ItemType>
	struct PickupAndDeliveryGoal : public DroneGoal
	{
		CellContent* deliveryDestination;
		unsigned int amount;

		PickupAndDeliveryGoal(Cell* _destination, Cell* _deliveryDestination, unsigned int _amount) 
			: DroneGoal(_destination), deliveryDestination(_deliveryDestination), amount(_amount) {}

		DroneGoal* destinationReached(entt::registry& registry, Drone& drone)
		{
			CellContent* content = destination->getContent();
			if (content == nullptr || deliveryDestination->cells.empty())
				return nullptr;

			entt::entity contentEntity = content->getEntity();
			IItem* item = ItemType().getFromEntity(registry, contentEntity);
			if (item == nullptr)
				return nullptr;

			drone.inventory.addItem(item->split(amount));
			
			return new DeliveryGoal(deliveryDestination->cells.begin()->first);
		}
	};

	struct DeliveryGoal : public DroneGoal
	{
		DeliveryGoal(Cell* _destination) : DroneGoal(_destination) {}

		DroneGoal* destinationReached(entt::registry& registry, Drone& drone);
	};
}
