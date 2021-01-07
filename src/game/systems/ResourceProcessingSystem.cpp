#include "ResourceProcessingSystem.hpp"

namespace game::systems
{
	std::unordered_set<IResourceProcessor*> resourceProcessors;

	void updateResourceProcessingSystem(entt::registry& registry, double deltaTime)
	{
		for (IResourceProcessor* resourceProcessor : resourceProcessors)
			resourceProcessor->processResources(registry, deltaTime);
	}

	void attachRessourceProcessor(IResourceProcessor* resourceProcessor)
	{
		resourceProcessors.insert(resourceProcessor);
	}
}
