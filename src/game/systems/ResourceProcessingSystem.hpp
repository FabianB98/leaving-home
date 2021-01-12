#pragma once

#include <limits>
#include <math.h>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <entt/entt.hpp>

#include <glm/glm.hpp>

#include "../world/Drone.hpp"
#include "../world/HeightGenerator.hpp"
#include "../world/Inventory.hpp"

namespace game::systems
{
	struct IResourceProcessor
	{
		virtual void processResources(entt::registry& registry, double deltaTime) = 0;
	};

	void updateResourceProcessingSystem(entt::registry& registry, double deltaTime, world::HeightGenerator& heightGenerator);

	void attachRessourceProcessor(IResourceProcessor* resourceProcessor);

	void registerItemType(std::shared_ptr<world::IItem> item);
}
