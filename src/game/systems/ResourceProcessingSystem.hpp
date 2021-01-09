#pragma once

#include <math.h>
#include <unordered_set>

#include <entt/entt.hpp>

#include "../world/Drone.hpp"
#include "../world/HeightGenerator.hpp"
#include "../world/Item.hpp"

namespace game::systems
{
	struct IResourceProcessor
	{
		virtual void processResources(entt::registry& registry, double deltaTime) = 0;
	};

	void updateResourceProcessingSystem(entt::registry& registry, double deltaTime, world::HeightGenerator& heightGenerator);

	void attachRessourceProcessor(IResourceProcessor* resourceProcessor);
}
