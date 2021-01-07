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
		DroneGoal* goal;
		std::vector<IItem*> carrying;

		static void spawnNewDrone(entt::registry& registry, const glm::vec3& position);
	};

	struct DroneGoal
	{
		Cell* destination;

		virtual DroneGoal* destinationReached(entt::registry& registry, Drone* drone) = 0;
	};

	struct MoveToGoal : public DroneGoal
	{
		DroneGoal* destinationReached(entt::registry& registry, Drone* drone);
	};

	struct HarvestGoal : public DroneGoal
	{
		DroneGoal* destinationReached(entt::registry& registry, Drone* drone);
	};

	template <class ItemType>
	struct PickupAndDeliveryGoal : public DroneGoal
	{
		CellContent* deliveryDestination;
		unsigned int amount;

		DroneGoal* destinationReached(entt::registry& registry, Drone* drone)
		{
			CellContent* content = destination->getContent();
			if (content == nullptr || deliveryDestination->cells.empty())
				return nullptr;

			entt::entity contentEntity = content->getEntity();

			// TODO: Pickup items.
			
			return new DeliveryGoal(deliveryDestination->cells.begin()->first);
		}
	};

	struct DeliveryGoal : public DroneGoal
	{
		DroneGoal* destinationReached(entt::registry& registry, Drone* drone);
	};
}
