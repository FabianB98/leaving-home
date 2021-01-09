#include "ResourceProcessingSystem.hpp"

namespace game::systems
{
	constexpr const float DRONE_MOVEMENT_SPEED = 10.0f;
	constexpr const float DRONE_FLIGHT_HEIGHT = 25.0f;

	std::unordered_set<IResourceProcessor*> resourceProcessors;

	void updateDrone(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		double deltaTime,
		world::HeightGenerator& heightGenerator
	) {
		if (drone.goal != nullptr)
		{
			// Drone has a goal which needs to be pursued
			auto& transform = registry.get<rendering::components::EulerComponentwiseTransform>(entity);
			glm::vec2 destination = drone.goal->destination->getRelaxedPosition();
			glm::vec2 currentPosition = glm::vec2(transform.getTranslation().x, transform.getTranslation().z);
			if (currentPosition == destination)
			{
				world::DroneGoal* nextGoal = drone.goal->destinationReached(registry, drone);
				delete drone.goal;
				drone.goal = nextGoal;
			}
			else
			{
				glm::vec2 deltaToDestination = destination - currentPosition;
				glm::vec2 direction = glm::normalize(deltaToDestination);
				float distanceToDestination = glm::length(deltaToDestination);

				float movementLength = DRONE_MOVEMENT_SPEED * (float)deltaTime;
				if (movementLength > distanceToDestination)
					movementLength = distanceToDestination;

				glm::vec2 deltaPos = direction * movementLength;
				glm::vec2 newPos = glm::vec2(transform.getTranslation().x, transform.getTranslation().z) + deltaPos;
				transform.setTranslation(glm::vec3(newPos.x, heightGenerator.getHeight(newPos) + DRONE_FLIGHT_HEIGHT, newPos.y));
				transform.setYaw(atan2(direction.x, direction.y));
			}
		}
		else if (!drone.inventory.items.empty())
		{
			// Drone has no goal, but stores items. Try to find a place where these items can be dropped of.
			auto item = *drone.inventory.items.begin();

			entt::entity entityToDeliverItemTo = item->getConsumes()->getAny(registry);
			if (entityToDeliverItemTo == entt::null)
				entityToDeliverItemTo = item->getStores()->getAny(registry);

			if (entityToDeliverItemTo != entt::null)
			{
				auto cellContentComponent = registry.get<world::CellContentComponent>(entityToDeliverItemTo);
				drone.goal = new world::DeliveryGoal(cellContentComponent.cellContent->getCells().begin()->first);
			}
		}
		else
		{
			// Drone has no goal and doesn't store items. Search for a new goal.
			// TODO: Search for a new goal.
		}
	}

	void updateResourceProcessingSystem(entt::registry& registry, double deltaTime, world::HeightGenerator& heightGenerator)
	{
		for (IResourceProcessor* resourceProcessor : resourceProcessors)
			resourceProcessor->processResources(registry, deltaTime);

		registry.view<world::Drone>().each([&registry, deltaTime, &heightGenerator](auto entity, auto& drone) {
			updateDrone(registry, entity, drone, deltaTime, heightGenerator);
		});
	}

	void attachRessourceProcessor(IResourceProcessor* resourceProcessor)
	{
		resourceProcessors.insert(resourceProcessor);
	}
}
