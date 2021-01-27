#pragma once

#include <vector>
#include <queue>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../rendering/bounding_geometry/Sphere.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../../rendering/systems/TransformHierarchySystem.hpp"
#include "Chunk.hpp"
#include "Inventory.hpp"

namespace game::world
{
	struct DroneTask;

	struct Drone
	{
		entt::entity rotor1Entity{ entt::null };
		entt::entity rotor2Entity{ entt::null };
		entt::entity rotor3Entity{ entt::null };
		entt::entity crateEntity{ entt::null };
		entt::entity spotLightEntity{ entt::null };

		float relativeWobbleSpeed{ 1.0f };
		float heightAboveGround{ DRONE_FLIGHT_HEIGHT };

		std::queue<DroneTask*> tasks;

		void inventoryUpdated(entt::registry& registry, entt::entity& entity, Inventory& inventory);

		static void spawnNewDrone(entt::registry& registry, const glm::vec3& position);
	};

	struct DroneTask
	{
		world::Cell* destination;

		DroneTask(world::Cell* _destination) : destination(_destination) {}

		virtual bool checkAndUpdateDestination(
			entt::registry& registry,
			entt::entity& entity,
			Drone& drone,
			Inventory& inventory
		) = 0;

		virtual bool destinationReached(
			entt::registry& registry,
			entt::entity& entity,
			Drone& drone,
			Inventory& inventory
		) = 0;
	};
}
