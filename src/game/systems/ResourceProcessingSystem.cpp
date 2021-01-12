#include "ResourceProcessingSystem.hpp"

namespace game::systems
{
	constexpr const float DRONE_MOVEMENT_SPEED = 10.0f;
	constexpr const float DRONE_FLIGHT_HEIGHT = 25.0f;

	std::unordered_set<IResourceProcessor*> resourceProcessors;
	std::unordered_set<std::shared_ptr<world::IItem>, world::IItemHash, world::IItemComparator> itemTypes;

	struct EntityAmount
	{
		entt::entity entity;
		std::shared_ptr<world::IItem> itemType;
		float amount;
	};

	struct MinPriorityQueue
	{
		bool operator() (const EntityAmount& a, const EntityAmount& b)
		{
			return a.amount > b.amount;
		}
	};

	struct MaxPriorityQueue
	{
		bool operator() (const EntityAmount& a, const EntityAmount& b)
		{
			return a.amount < b.amount;
		}
	};

	std::unordered_map<std::shared_ptr<world::IItem>, std::priority_queue<EntityAmount, std::vector<EntityAmount>, MinPriorityQueue>, world::IItemHash, world::IItemComparator> deliveryGoalsItemwise;
	std::unordered_map<std::shared_ptr<world::IItem>, std::priority_queue<EntityAmount, std::vector<EntityAmount>, MaxPriorityQueue>, world::IItemHash, world::IItemComparator> filledStoragesItemwise;
	std::unordered_map<std::shared_ptr<world::IItem>, std::priority_queue<EntityAmount, std::vector<EntityAmount>, MaxPriorityQueue>, world::IItemHash, world::IItemComparator> filledProducersItemwise;
	std::priority_queue<EntityAmount, std::vector<EntityAmount>, MaxPriorityQueue> filledProducers;
	std::priority_queue<EntityAmount, std::vector<EntityAmount>, MinPriorityQueue> starvingConsumers;

	world::Cell* findNearestCell(
		entt::registry& registry,
		entt::entity& entity,
		world::CellContent* cellContent
	) {
		world::Cell* result = nullptr;
		float currentSquaredDistance = std::numeric_limits<float>::max();

		auto& transform = registry.get<rendering::components::EulerComponentwiseTransform>(entity);
		glm::vec2 dronePosition = glm::vec2(transform.getTranslation().x, transform.getTranslation().z);

		for (auto cell : cellContent->getCells())
		{
			float squaredDistanceToCell = glm::distance2(dronePosition, cell.first->getRelaxedPosition());
			if (squaredDistanceToCell < currentSquaredDistance)
			{
				result = cell.first;
				currentSquaredDistance = squaredDistanceToCell;
			}
		}

		return result;
	}

	bool findSourceForStarvingConsumer(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		EntityAmount& consumer,
		std::priority_queue<EntityAmount, std::vector<EntityAmount>, MaxPriorityQueue>& sources
	) {
		while (!sources.empty())
		{
			EntityAmount source = sources.top();
			sources.pop();

			world::CellContent* sourceCellContent = registry.get<world::CellContentComponent>(source.entity).cellContent;
			world::CellContent* consumerCellContent = registry.get<world::CellContentComponent>(consumer.entity).cellContent;
			drone.goal = new world::PickupAndDeliveryGoal(
				findNearestCell(registry, entity, sourceCellContent),
				findNearestCell(registry, entity, consumerCellContent),
				consumer.itemType,
				10.0f);

			return true;
		}

		return false;
	}

	void findGoal(entt::registry& registry, entt::entity& entity, world::Drone& drone) {
		// TODO: Add build and harvest goals.

		while (!starvingConsumers.empty())
		{
			EntityAmount consumer = starvingConsumers.top();
			starvingConsumers.pop();

			if (findSourceForStarvingConsumer(registry, entity, drone, consumer, filledProducersItemwise[consumer.itemType]))
				return;

			if (findSourceForStarvingConsumer(registry, entity, drone, consumer, filledStoragesItemwise[consumer.itemType]))
				return;
		}

		while (!filledProducers.empty())
		{
			EntityAmount producer = filledProducers.top();
			filledProducers.pop();

			if (!deliveryGoalsItemwise[producer.itemType].empty())
			{
				EntityAmount destination = deliveryGoalsItemwise[producer.itemType].top();
				deliveryGoalsItemwise[producer.itemType].pop();

				world::CellContent* producerCellContent = registry.get<world::CellContentComponent>(producer.entity).cellContent;
				world::CellContent* destinationCellContent = registry.get<world::CellContentComponent>(destination.entity).cellContent;
				drone.goal = new world::PickupAndDeliveryGoal(
					findNearestCell(registry, entity, producerCellContent),
					findNearestCell(registry, entity, destinationCellContent),
					producer.itemType,
					10.0f);

				return;
			}
		}
	}

	void updateDrone(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		double deltaTime,
		world::HeightGenerator& heightGenerator
	) {
		if (drone.goal != nullptr && drone.goal->destination->getContent() == nullptr)
		{
			// Drone has a goal, but its destination cell doesn't have a CellContent (most likely because it was removed
			// from that cell). Therefore, this goal can't be pursued anymore.
			delete drone.goal;
			drone.goal = nullptr;
		}

		if (drone.goal != nullptr)
		{
			// Drone has a goal which needs to be pursued.
			auto& transform = registry.get<rendering::components::EulerComponentwiseTransform>(entity);
			glm::vec2 destination = drone.goal->destination->getRelaxedPosition();
			glm::vec2 currentPosition = glm::vec2(transform.getTranslation().x, transform.getTranslation().z);
			if (currentPosition == destination)
			{
				// Drone has reached its destination. Perform the intended action.
				world::DroneGoal* nextGoal = drone.goal->destinationReached(registry, drone);
				delete drone.goal;
				drone.goal = nextGoal;
			}
			else
			{
				// Drone has not yet reached its destination. Move towards the destination position.
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
			if (!deliveryGoalsItemwise[item].empty())
			{
				entt::entity entityToDeliverItemTo = deliveryGoalsItemwise[item].top().entity;
				deliveryGoalsItemwise[item].pop();

				auto cellContentComponent = registry.get<world::CellContentComponent>(entityToDeliverItemTo);
				drone.goal = new world::DeliveryGoal(findNearestCell(registry, entity, cellContentComponent.cellContent));
			}
		}
		else
		{
			// Drone has no goal and doesn't store items. Search for a new goal.
			findGoal(registry, entity, drone);
		}
	}

	void updateResourceProcessingSystem(entt::registry& registry, double deltaTime, world::HeightGenerator& heightGenerator)
	{
		for (IResourceProcessor* resourceProcessor : resourceProcessors)
			resourceProcessor->processResources(registry, deltaTime);

		deliveryGoalsItemwise.clear();
		filledStoragesItemwise.clear();
		filledProducersItemwise.clear();
		filledProducers = std::priority_queue<EntityAmount, std::vector<EntityAmount>, MaxPriorityQueue>();
		starvingConsumers = std::priority_queue<EntityAmount, std::vector<EntityAmount>, MinPriorityQueue>();

		for (std::shared_ptr<world::IItem> itemType : itemTypes)
		{
			itemType->getConsumes()->iterateAllEntities(registry, std::function([&registry, itemType](entt::entity& entity, world::IConsumes* consumes) {
				std::shared_ptr<world::IItem> storedItem = registry.get<world::Inventory>(entity).getItem(itemType);
				float storedAmount = storedItem != nullptr ? storedItem->amount : 0.0f;

				EntityAmount entityAmount{ entity, itemType, storedAmount };
				deliveryGoalsItemwise[itemType].push(entityAmount);
				if (storedAmount <= 10.0f)
					starvingConsumers.push(entityAmount);
			}));

			itemType->getStores()->iterateAllEntities(registry, std::function([&registry, itemType](entt::entity& entity, world::IStores* stores) {
				std::shared_ptr<world::IItem> storedItem = registry.get<world::Inventory>(entity).getItem(itemType);
				float storedAmount = storedItem != nullptr ? storedItem->amount : 0.0f;

				EntityAmount entityAmount{ entity, itemType, storedAmount };
				deliveryGoalsItemwise[itemType].push(entityAmount);
				if (storedAmount != 0.0f)
					filledStoragesItemwise[itemType].push(entityAmount);
			}));

			itemType->getProduces()->iterateAllEntities(registry, std::function([&registry, itemType](entt::entity& entity, world::IProduces* produces) {
				std::shared_ptr<world::IItem> storedItem = registry.get<world::Inventory>(entity).getItem(itemType);
				if (storedItem == nullptr || storedItem->amount == 0.0f)
					return;

				EntityAmount entityAmount{ entity, itemType, storedItem->amount };
				filledProducersItemwise[itemType].push(entityAmount);
				filledProducers.push(entityAmount);
			}));
		}

		registry.view<world::Drone>().each([&registry, deltaTime, &heightGenerator](auto entity, auto& drone) {
			updateDrone(registry, entity, drone, deltaTime, heightGenerator);
		});
	}

	void attachRessourceProcessor(IResourceProcessor* resourceProcessor)
	{
		resourceProcessors.insert(resourceProcessor);
	}

	void registerItemType(std::shared_ptr<world::IItem> item)
	{
		itemTypes.insert(item);
	}
}
