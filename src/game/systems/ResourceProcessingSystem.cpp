#include "ResourceProcessingSystem.hpp"

namespace game::systems
{
	std::unordered_set<IResourceProcessor*> resourceProcessors;
	std::unordered_set<std::shared_ptr<world::IItem>, world::IItemHash, world::IItemComparator> itemTypes;

	std::queue<std::pair<world::Cell*, world::IBuilding*>> buildingsToPlace;
	std::queue<world::Cell*> buildingsToRemove;

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

	template <typename Comparator>
	world::CellContent* findCellForItemType(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		std::shared_ptr<world::IItem> itemType,
		std::priority_queue<EntityAmount, std::vector<EntityAmount>, Comparator>& sources
	) {
		while (!sources.empty())
		{
			EntityAmount source = sources.top();
			sources.pop();

			return registry.get<world::CellContentComponent>(source.entity).cellContent;
		}

		return nullptr;
	}

	world::CellContent* findPickupCellContent(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		std::shared_ptr<world::IItem> itemType
	) {
		world::CellContent* result = findCellForItemType(registry, entity, drone, itemType, filledProducersItemwise[itemType]);
		if (result != nullptr)
			return result;

		return findCellForItemType(registry, entity, drone, itemType, filledStoragesItemwise[itemType]);
	}

	world::CellContent* findDeliveryCellContent(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		std::shared_ptr<world::IItem> itemType
	) {
		return findCellForItemType(registry, entity, drone, itemType, deliveryGoalsItemwise[itemType]);
	}

	void setDestination(
		world::DroneTask* task,
		world::CellContent* destination,
		entt::registry& registry,
		entt::entity& entity
	) {
		if (destination != nullptr)
			task->destination = findNearestCell(registry, entity, destination);
		else
			task->destination = nullptr;
	}

	struct PickupTask : public world::DroneTask
	{
		std::shared_ptr<world::IItem> itemType;
		float amount;

		PickupTask(world::Cell* _destination, std::shared_ptr<world::IItem> _itemType, float _amount)
			: world::DroneTask(_destination), itemType(itemType), amount(_amount) {}

		bool checkAndUpdateDestination(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			if (destination == nullptr || destination->getContent() == nullptr)
			{
				// Destination is no longer valid. Find a new destination from which the items can be picked up.
				setDestination(this, findPickupCellContent(registry, entity, drone, itemType), registry, entity);
			}

			return true;
		}

		bool destinationReached(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			entt::entity contentEntity = destination->getContent()->getEntity();
			world::Inventory& contentInventory = registry.get<world::Inventory>(contentEntity);

			std::vector<std::shared_ptr<world::IItem>> itemsToRemoveFromInventory;
			for (std::shared_ptr<world::IItem> item : contentInventory.items)
			{
				bool harvestableItem = item->getHarvestable()->getFromEntity(registry, contentEntity) != nullptr;
				bool storesItem = item->getStores()->getFromEntity(registry, contentEntity) != nullptr;
				bool producesItem = item->getProduces()->getFromEntity(registry, contentEntity) != nullptr;

				if (harvestableItem || storesItem || producesItem)
				{
					inventory.addItem(item);
					itemsToRemoveFromInventory.push_back(item);
				}
			}

			for (std::shared_ptr<world::IItem> item : itemsToRemoveFromInventory)
				contentInventory.items.erase(item);

			drone.inventoryUpdated(registry, entity, inventory);

			return true;
		}
	};

	struct DeliveryTask : public world::DroneTask
	{
		DeliveryTask(world::Cell* _destination) : world::DroneTask(_destination) {}

		bool checkAndUpdateDestination(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			if (inventory.items.empty())
				return false;

			if (destination == nullptr || destination->getContent() == nullptr)
			{
				// Destination is no longer valid. Find a new destination where the items can be delivered at.
				auto itemType = *inventory.items.begin();
				setDestination(this, findDeliveryCellContent(registry, entity, drone, itemType), registry, entity);
			}

			return true;
		}

		bool destinationReached(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			entt::entity contentEntity = destination->getContent()->getEntity();
			world::Inventory& contentInventory = registry.get<world::Inventory>(contentEntity);

			std::vector<std::shared_ptr<world::IItem>> itemsToRemoveFromInventory;
			for (std::shared_ptr<world::IItem> item : inventory.items)
			{
				bool consumesItem = item->getConsumes()->getFromEntity(registry, contentEntity) != nullptr;
				bool storesItem = item->getStores()->getFromEntity(registry, contentEntity) != nullptr;

				if (consumesItem || storesItem)
				{
					contentInventory.addItem(item);
					itemsToRemoveFromInventory.push_back(item);
				}
			}

			for (std::shared_ptr<world::IItem> item : itemsToRemoveFromInventory)
				inventory.items.erase(item);

			drone.inventoryUpdated(registry, entity, inventory);

			return true;
		}
	};

	struct ConstructionTask : public world::DroneTask
	{
		world::IBuilding* buildingType;

		ConstructionTask(world::Cell* _destination, world::IBuilding* _buildingType) : world::DroneTask(_destination), buildingType(_buildingType)
		{
			if (destination == nullptr)
				throw std::logic_error("ConstructionTask created with no destination! This must be a bug...");
		}

		bool checkAndUpdateDestination(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			return true;
		}

		bool destinationReached(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			// TODO: Remove the required items to construct the building from the drone's inventory.

			buildingType->placeBuildingOfThisTypeOnCell(destination);

			return true;
		}
	};

	struct DestructionTask : public world::DroneTask
	{
		DestructionTask(world::Cell* _destination) : world::DroneTask(_destination)
		{
			if (destination == nullptr)
				throw std::logic_error("DestructionTask created with no destination! This must be a bug...");
		}

		bool checkAndUpdateDestination(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			return destination->getContent() != nullptr;
		}

		bool destinationReached(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			// TODO: Move items from the destroyed CellContent to the drone's inventory.

			destination->setContent(nullptr);

			return true;
		}
	};

	void findGoal(entt::registry& registry, entt::entity& entity, world::Drone& drone) {
		while (!buildingsToPlace.empty())
		{
			// TODO: Check whether all resources required to place the building can be taken from somewhere to ensure
			// that the drone won't get stuck trying to get a resource.

			std::pair<world::Cell*, world::IBuilding*>& cellAndBuilding = buildingsToPlace.front();
			// TODO: Add tasks for getting all required resources.
			drone.tasks.push(new ConstructionTask(cellAndBuilding.first, cellAndBuilding.second));
			buildingsToPlace.pop();

			return;
		}

		while (!buildingsToRemove.empty())
		{
			drone.tasks.push(new DestructionTask(buildingsToRemove.front()));
			buildingsToRemove.pop();

			return;
		}

		while (!starvingConsumers.empty())
		{
			EntityAmount consumer = starvingConsumers.top();
			starvingConsumers.pop();

			world::CellContent* sourceCellContent = findPickupCellContent(registry, entity, drone, consumer.itemType);
			if (sourceCellContent != nullptr)
			{
				drone.tasks.push(new PickupTask(findNearestCell(registry, entity, sourceCellContent), consumer.itemType, 10.0f));

				world::CellContent* consumerCellContent = registry.get<world::CellContentComponent>(consumer.entity).cellContent;
				drone.tasks.push(new DeliveryTask(findNearestCell(registry, entity, consumerCellContent)));

				return;
			}
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
				drone.tasks.push(new PickupTask(findNearestCell(registry, entity, producerCellContent), producer.itemType, 10.0f));

				world::CellContent* destinationCellContent = registry.get<world::CellContentComponent>(destination.entity).cellContent;
				drone.tasks.push(new DeliveryTask(findNearestCell(registry, entity, destinationCellContent)));

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
		auto& transform = registry.get<rendering::components::EulerComponentwiseTransform>(entity);
		auto& inventory = registry.get<world::Inventory>(entity);

		if (!drone.tasks.empty())
		{
			// Drone has a task which needs to be pursued.
			world::DroneTask* task = drone.tasks.front();
			if (task->checkAndUpdateDestination(registry, entity, drone, inventory))
			{
				// Task is still valid and therefore needs to be pursued.
				if (task->destination != nullptr)
				{
					// Task's destination is valid. Move to the destination and perform the task's action.
					glm::vec2 destination = task->destination->getRelaxedPosition();
					glm::vec2 currentPosition = glm::vec2(transform.getTranslation().x, transform.getTranslation().z);
					if (currentPosition == destination)
					{
						// Drone has reached its destination. Perform the intended action.
						if (task->destinationReached(registry, entity, drone, inventory))
						{
							// Action was performed successfully. We can now delete it from the drone's tasks.
							drone.tasks.pop();
							delete task;
						}
					}
					else
					{
						// Drone has not yet reached its destination. Move towards the destination position.
						glm::vec2 deltaToDestination = destination - currentPosition;
						glm::vec2 direction = glm::normalize(deltaToDestination);
						float distanceToDestination = glm::length(deltaToDestination);

						float movementLength = world::DRONE_MOVEMENT_SPEED * (float)deltaTime;
						if (movementLength > distanceToDestination)
							movementLength = distanceToDestination;

						glm::vec2 deltaPos = direction * movementLength;
						glm::vec3 translation = transform.getTranslation();
						glm::vec2 newPos = glm::vec2(translation.x, translation.z) + deltaPos;
						transform.setTranslation(glm::vec3(newPos.x, 0.0f, newPos.y));
						transform.setYaw(atan2(direction.x, direction.y));
					}
				}
			}
			else
			{
				// The current task is no longer valid and can therefore be no longer pursued. Delete it from the
				// drone's tasks and continue with the next task on the next update.
				drone.tasks.pop();
				delete task;
			}
		}
		else if (!inventory.items.empty())
		{
			// Drone has no goal, but stores items. Try to find a place where these items can be dropped of.
			auto item = *inventory.items.begin();
			if (!deliveryGoalsItemwise[item].empty())
			{
				entt::entity entityToDeliverItemTo = deliveryGoalsItemwise[item].top().entity;
				deliveryGoalsItemwise[item].pop();

				auto cellContentComponent = registry.get<world::CellContentComponent>(entityToDeliverItemTo);
				drone.tasks.push(new DeliveryTask(findNearestCell(registry, entity, cellContentComponent.cellContent)));
			}
		}
		else
		{
			// Drone has no goal and doesn't store items. Search for a new goal.
			findGoal(registry, entity, drone);
		}

		// Let the drone wobble slightly up and down to make its flight look more realistic.
		double time = glfwGetTime();
		glm::vec3 translation = transform.getTranslation();
		float height = heightGenerator.getHeight(translation.x, translation.z)
			+ drone.heightAboveGround
			+ world::DRONE_WOBBLE_HEIGHT * sin(time * world::DRONE_WOBBLE_SPEED * drone.relativeWobbleSpeed);
		transform.setTranslation(glm::vec3(translation.x, height, translation.z));

		// Update the rotation of the drone's rotors.
		float rotation = fmodf(time * world::DRONE_ROTOR_ROTATION_SPEED, 2.0f * M_PI);
		registry.get<rendering::components::EulerComponentwiseTransform>(drone.rotor1Entity).setYaw(rotation);
		registry.get<rendering::components::EulerComponentwiseTransform>(drone.rotor2Entity).setYaw(rotation);
		registry.get<rendering::components::EulerComponentwiseTransform>(drone.rotor3Entity).setYaw(rotation);
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

	void enqueueConstruction(world::Cell* cell, world::IBuilding* buildingType)
	{
		buildingsToPlace.push(std::make_pair(cell, buildingType));
	}

	void enqueueDestruction(world::Cell* cell)
	{
		buildingsToRemove.push(cell);
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
