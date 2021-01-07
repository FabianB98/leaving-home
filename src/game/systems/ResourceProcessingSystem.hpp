#pragma once

#include <unordered_set>

#include <entt/entt.hpp>

namespace game::systems
{
	struct IResourceProcessor
	{
		virtual void processResources(entt::registry& registry, double deltaTime) = 0;
	};

	void updateResourceProcessingSystem(entt::registry& registry, double deltaTime);

	void attachRessourceProcessor(IResourceProcessor* resourceProcessor);
}
