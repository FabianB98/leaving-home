#pragma once

#include <limits>
#include <math.h>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <entt/entt.hpp>

#include <glm/glm.hpp>

#include "../world/BuildingPieceSet.hpp"
#include "../world/Chunk.hpp"
#include "../world/Constants.hpp"
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

	void enqueueConstruction(world::Cell* cell, world::IBuilding* buildingType);

	void enqueueDestruction(world::Cell* cell);

	void attachResourceProcessor(IResourceProcessor* resourceProcessor);

	void registerItemType(std::shared_ptr<world::IItem> item);
}
