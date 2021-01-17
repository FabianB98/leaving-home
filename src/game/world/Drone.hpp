#pragma once

#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../rendering/bounding_geometry/Sphere.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../../rendering/systems/TransformHierarchySystem.hpp"
#include "Chunk.hpp"
#include "Inventory.hpp"

namespace game::world
{
	struct DroneGoal;

	struct Drone
	{
		entt::entity rotor1Entity{ entt::null };
		entt::entity rotor2Entity{ entt::null };
		entt::entity rotor3Entity{ entt::null };
		entt::entity crateEntity{ entt::null };

		float relativeWobbleSpeed{ 1.0f };
		float heightAboveGround{ DRONE_FLIGHT_HEIGHT };

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

	struct PickupAndDeliveryGoal : public DroneGoal
	{
		Cell* deliveryDestination;
		std::shared_ptr<world::IItem> itemType;
		float amount;

		PickupAndDeliveryGoal(
			Cell* _destination,
			Cell* _deliveryDestination,
			std::shared_ptr<world::IItem> _itemType,
			float _amount
		) : DroneGoal(_destination), deliveryDestination(_deliveryDestination), itemType(_itemType), amount(_amount) {}

		DroneGoal* destinationReached(entt::registry& registry, Drone& drone);
	};

	struct DeliveryGoal : public DroneGoal
	{
		DeliveryGoal(Cell* _destination) : DroneGoal(_destination) {}

		DroneGoal* destinationReached(entt::registry& registry, Drone& drone);
	};
}
