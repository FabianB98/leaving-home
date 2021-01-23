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

	std::priority_queue<EntityAmount, std::vector<EntityAmount>, MaxPriorityQueue> filledProducers;
	std::priority_queue<EntityAmount, std::vector<EntityAmount>, MinPriorityQueue> starvingConsumers;

	struct PlannedInventoryChanges
	{
		world::Inventory plannedPickups;
		world::Inventory plannedDeliveries;
	};

	std::unordered_map<entt::entity, PlannedInventoryChanges> plannedInventoryChanges;

	struct PlannedInventoryChange
	{
		entt::entity contentEntity;
		std::shared_ptr<world::IItem> itemAndAmount;
		bool finished;

		PlannedInventoryChange(entt::entity _contentEntity, std::shared_ptr<world::IItem> _itemAndAmount)
			: contentEntity(_contentEntity), itemAndAmount(_itemAndAmount), finished(false) {}

		float execute(entt::registry& registry, entt::entity droneEntity, entt::entity destinationEntity)
		{
			if (finished)
				return 0.0f;

			entt::entity entityToInteractWith = contentEntity;
			if (!registry.valid(contentEntity) || contentEntity != destinationEntity)
				entityToInteractWith = destinationEntity;

			world::Inventory& contentInventory = registry.get<world::Inventory>(entityToInteractWith);
			world::Inventory& droneInventory = registry.get<world::Inventory>(droneEntity);

			world::Inventory& sourceInventory = getSourceInventory(contentInventory, droneInventory);
			world::Inventory& destinationInventory = getDestinationInventory(contentInventory, droneInventory);

			std::shared_ptr<world::IItem> item = sourceInventory.removeItem(itemAndAmount, itemAndAmount->amount);
			if (item == nullptr)
			{
				cancel(registry);
				return 0.0f;
			}
			destinationInventory.addItem(item);

			registry.get<world::CellContentComponent>(entityToInteractWith).cellContent->inventoryUpdated();
			registry.get<world::Drone>(droneEntity).inventoryUpdated(registry, droneEntity, droneInventory);

			cancel(registry);

			return item->amount;
		}

		void cancel(entt::registry& registry)
		{
			if (finished)
				return;

			getPlanningInventory().removeItem(itemAndAmount, itemAndAmount->amount);
			if (!registry.valid(contentEntity))
				plannedInventoryChanges.erase(contentEntity);

			finished = true;
		}

		virtual world::Inventory& getPlanningInventory() = 0;

		virtual world::Inventory& getSourceInventory(
			world::Inventory& contentInventory,
			world::Inventory& droneInventory
		) = 0;

		virtual world::Inventory& getDestinationInventory(
			world::Inventory& contentInventory,
			world::Inventory& droneInventory
		) = 0;
	};

	struct PlannedPickup : public PlannedInventoryChange
	{
		PlannedPickup(entt::entity _contentEntity, std::shared_ptr<world::IItem> _itemAndAmount)
			: PlannedInventoryChange(_contentEntity, _itemAndAmount)
		{
			getPlanningInventory().addItem(itemAndAmount->clone());
		}

		PlannedPickup(const PlannedPickup& other) : PlannedInventoryChange(other.contentEntity, other.itemAndAmount) {}

		world::Inventory& getPlanningInventory()
		{
			return plannedInventoryChanges[contentEntity].plannedPickups;
		}

		world::Inventory& getSourceInventory(world::Inventory& contentInventory, world::Inventory& droneInventory)
		{
			return contentInventory;
		}

		world::Inventory& getDestinationInventory(world::Inventory& contentInventory, world::Inventory& droneInventory)
		{
			return droneInventory;
		}
	};

	struct PlannedDelivery : public PlannedInventoryChange
	{
		PlannedDelivery(entt::entity _contentEntity, std::shared_ptr<world::IItem> _itemAndAmount)
			: PlannedInventoryChange(_contentEntity, _itemAndAmount)
		{
			getPlanningInventory().addItem(itemAndAmount->clone());
		}

		PlannedDelivery(const PlannedPickup& other) : PlannedInventoryChange(other.contentEntity, other.itemAndAmount) {}

		world::Inventory& getPlanningInventory()
		{
			return plannedInventoryChanges[contentEntity].plannedPickups;
		}

		world::Inventory& getSourceInventory(world::Inventory& contentInventory, world::Inventory& droneInventory)
		{
			return droneInventory;
		}

		world::Inventory& getDestinationInventory(world::Inventory& contentInventory, world::Inventory& droneInventory)
		{
			return contentInventory;
		}
	};

	world::Cell* findNearestCell(glm::vec2 dronePosition, world::CellContent* destination)
	{
		world::Cell* result = nullptr;
		float currentSquaredDistance = std::numeric_limits<float>::max();

		for (auto cell : destination->getCells())
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

	world::Cell* findNearestCell(world::Cell* startCell, world::CellContent* destination)
	{
		return findNearestCell(startCell->getRelaxedPosition(), destination);
	}

	world::Cell* findNearestCell(
		entt::registry& registry,
		entt::entity& entity,
		world::CellContent* destination
	) {
		auto& transform = registry.get<rendering::components::EulerComponentwiseTransform>(entity);
		glm::vec2 dronePosition = glm::vec2(transform.getTranslation().x, transform.getTranslation().z);

		return findNearestCell(dronePosition, destination);
	}

	template <class ItemInteraction>
	void findBestCandidate(
		float& currentBestScore,
		world::CellContent*& bestMatch,
		entt::registry& registry,
		glm::vec2 dronePos,
		std::shared_ptr<world::IItem> item,
		std::function<float(world::Inventory&, PlannedInventoryChanges*)>& getItemScore,
		ItemInteraction* candidates
	) {
		candidates->iterateAllEntities(registry, std::function([&currentBestScore, &bestMatch, &registry, item, &getItemScore, dronePos](entt::entity& candidate, ItemInteraction* interaction) {
			// Calculate the distance from the drone to the candidate.
			world::CellContent* candidateCellContent = registry.get<world::CellContentComponent>(candidate).cellContent;
			world::Cell* nearestCell = findNearestCell(dronePos, candidateCellContent);
			float distanceScore = -glm::distance(dronePos, nearestCell->getRelaxedPosition());

			// Calculate the candidate's item score (i.e. how preferred the candidate would be selected based on the amount
			// of stored items and planned changes to the candidate's inventory).
			world::Inventory& candidateInventory = registry.get<world::Inventory>(candidate);
			PlannedInventoryChanges* plannedChanges = nullptr;
			auto& found = plannedInventoryChanges.find(candidate);
			if (found != plannedInventoryChanges.end())
				plannedChanges = &found->second;
			float itemScore = getItemScore(candidateInventory, plannedChanges);
			if (itemScore == std::numeric_limits<float>::lowest())
				return;

			// Check if the current candidate is better than the current best, and update the current best accordingly.
			float totalScore = distanceScore * world::RESOURCE_MANAGEMENT_CANDIDATE_PLANNING_DISTANCE_WEIGHT
				+ itemScore * world::RESOURCE_MANAGEMENT_CANDIDATE_PLANNING_ITEMS_WEIGHT;
			if (totalScore > currentBestScore)
			{
				currentBestScore = totalScore;
				bestMatch = candidateCellContent;
			}
		}));
	}

	world::CellContent* findPickupCellContent(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		std::shared_ptr<world::IItem> item,
		bool checkHarvestables
	) {
		auto& droneTransform = registry.get<rendering::components::EulerComponentwiseTransform>(entity);
		glm::vec2 dronePos = glm::vec2(droneTransform.getTranslation().x, droneTransform.getTranslation().z);

		auto getItemScore = std::function([item](world::Inventory& inventory, PlannedInventoryChanges* plannedChanges) -> float {
			float stored = inventory.getStoredAmount(item);

			float plannedForPickup = 0.0f;
			if (plannedChanges != nullptr)
				plannedForPickup = plannedChanges->plannedPickups.getStoredAmount(item);

			float remainingAmount = stored - plannedForPickup;
			if (remainingAmount <= 0.0f)
				return std::numeric_limits<float>::lowest();
			else
				return remainingAmount;
		});

		float currentBestScore = std::numeric_limits<float>::lowest();
		world::CellContent* bestMatch = nullptr;
		findBestCandidate(currentBestScore, bestMatch, registry, dronePos, item, getItemScore, item->getProduces());
		findBestCandidate(currentBestScore, bestMatch, registry, dronePos, item, getItemScore, item->getStores());

		if (checkHarvestables && bestMatch == nullptr)
			findBestCandidate(currentBestScore, bestMatch, registry, dronePos, item, getItemScore, item->getHarvestable());

		return bestMatch;
	}

	world::CellContent* findDeliveryCellContent(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		std::shared_ptr<world::IItem> itemType
	) {
		auto& droneTransform = registry.get<rendering::components::EulerComponentwiseTransform>(entity);
		glm::vec2 dronePos = glm::vec2(droneTransform.getTranslation().x, droneTransform.getTranslation().z);

		auto getItemScore = std::function([itemType](world::Inventory& inventory, PlannedInventoryChanges* plannedChanges) -> float {
			float stored = inventory.getStoredAmount(itemType);

			float plannedForDelivery = 0.0f;
			if (plannedChanges != nullptr)
				plannedForDelivery = plannedChanges->plannedDeliveries.getStoredAmount(itemType);

			// Consumers with less stored items should be preferred over consumers with more stored items, hence we're inverting
			// the planned amount before returning it so that consumers with less stored items will get a greater score.
			float plannedAmount = stored + plannedForDelivery;
			return -plannedAmount;
		});

		float currentBestScore = std::numeric_limits<float>::lowest();
		world::CellContent* bestMatch = nullptr;
		findBestCandidate(currentBestScore, bestMatch, registry, dronePos, itemType, getItemScore, itemType->getConsumes());

		return bestMatch;
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
		std::shared_ptr<world::IItem> item;
		bool exact;
		bool checkHarvestables;
		PlannedPickup plannedPickup;

		PickupTask(world::Cell* _destination, std::shared_ptr<world::IItem> _item, bool _exact, bool _checkHarvestables)
			: world::DroneTask(_destination),
			item(_item),
			exact(_exact),
			checkHarvestables(_checkHarvestables),
			plannedPickup(PlannedPickup(destination->getContent()->getEntity(), item)) {}

		bool checkAndUpdateDestination(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			if (destination == nullptr || destination->getContent() == nullptr)
			{
				// Destination is no longer valid. Find a new destination from which the items can be picked up.
				plannedPickup.cancel(registry);
				setDestination(this, findPickupCellContent(registry, entity, drone, item, checkHarvestables), registry, entity);
				plannedPickup = PlannedPickup(destination->getContent()->getEntity(), item);
			}

			return true;
		}

		bool destinationReached(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			float amountPickedUp = plannedPickup.execute(registry, entity, destination->getContent()->getEntity());
			if (exact && amountPickedUp != item->amount)
			{
				item->amount -= amountPickedUp;
				destination = nullptr;
				return false;
			}

			return true;
		}

		void cancel(entt::registry& registry)
		{
			plannedPickup.cancel(registry);
		}
	};

	struct DeliveryTask : public world::DroneTask
	{
		std::shared_ptr<world::IItem> item;
		PlannedDelivery plannedDelivery;

		DeliveryTask(world::Cell* _destination, std::shared_ptr<world::IItem> _item)
			: world::DroneTask(_destination), item(_item), plannedDelivery(PlannedDelivery(destination->getContent()->getEntity(), item)) {}

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
				plannedDelivery.cancel(registry);
				setDestination(this, findDeliveryCellContent(registry, entity, drone, item), registry, entity);
				plannedDelivery = PlannedDelivery(destination->getContent()->getEntity(), item);
			}

			return true;
		}

		bool destinationReached(
			entt::registry& registry,
			entt::entity& entity,
			world::Drone& drone,
			world::Inventory& inventory
		) {
			plannedDelivery.execute(registry, entity, destination->getContent()->getEntity());
			return true;
		}

		void cancel(entt::registry& registry)
		{
			plannedDelivery.cancel(registry);
		}
	};

	struct ConstructionTask : public world::DroneTask
	{
		world::IBuilding* buildingType;

		ConstructionTask(world::Cell* _destination, world::IBuilding* _buildingType) : world::DroneTask(_destination), buildingType(_buildingType)
		{
			if (destination == nullptr)
				throw std::logic_error("ConstructionTask created with no destination! This must be a bug in the task planning algorithm...");
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
			for (std::shared_ptr<world::IItem> item : buildingType->getResourcesRequiredToBuild().items)
				inventory.removeItem(item, item->amount);
			drone.inventoryUpdated(registry, entity, inventory);

			buildingType->placeBuildingOfThisTypeOnCell(destination);

			return true;
		}
	};

	struct DestructionTask : public world::DroneTask
	{
		DestructionTask(world::Cell* _destination) : world::DroneTask(_destination)
		{
			if (destination == nullptr)
				throw std::logic_error("DestructionTask created with no destination! This must be a bug in the task planning algorithm...");
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
			world::CellContent* destinationContent = destination->getContent();
			inventory.addItems(destinationContent->getResourcesObtainedByRemoval(destination));

			destinationContent->inventoryUpdated();
			drone.inventoryUpdated(registry, entity, inventory);

			destination->setContent(nullptr);

			return true;
		}
	};

	float calculateAmountToPickup(
		entt::registry& registry,
		world::CellContent* sourceCellContent,
		std::shared_ptr<world::IItem> itemType,
		float maxAmountToPickup
	) {
		float stored = registry.get<world::Inventory>(sourceCellContent->getEntity()).getStoredAmount(itemType);

		float plannedForPickup = 0.0f;
		auto& found = plannedInventoryChanges.find(sourceCellContent->getEntity());
		if (found != plannedInventoryChanges.end())
			plannedForPickup = found->second.plannedPickups.getStoredAmount(itemType);

		float availableAmount = stored - plannedForPickup;
		return std::min(availableAmount, maxAmountToPickup);
	}

	void schedulePickupAndDeliveryTask(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		std::shared_ptr<world::IItem> itemType,
		world::CellContent* sourceCellContent,
		world::CellContent* destinationCellContent
	) {
		float amountToPickup = calculateAmountToPickup(
			registry,
			sourceCellContent,
			itemType,
			world::RESOURCE_MANAGEMENT_DEFAULT_TRANSPORT_CAPACITY
		);

		std::shared_ptr<world::IItem> itemAndAmount = itemType->clone(amountToPickup);
		world::Cell* pickupCell = findNearestCell(registry, entity, sourceCellContent);
		drone.tasks.push(new PickupTask(pickupCell, itemAndAmount, false, false));
		drone.tasks.push(new DeliveryTask(findNearestCell(pickupCell, destinationCellContent), itemAndAmount));
	}

	bool tryScheduleConstructionTask(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		std::pair<world::Cell*, world::IBuilding*>& cellAndBuilding
	) {
		std::vector<PickupTask*> pickupTasks;
		world::Cell* lastCell = nullptr;
		for (std::shared_ptr<world::IItem> item : cellAndBuilding.second->getResourcesRequiredToBuild().items)
		{
			float remainingAmountToPickup = item->amount - registry.get<world::Inventory>(entity).getStoredAmount(item);
			while (remainingAmountToPickup > 0.0f)
			{
				world::CellContent* sourceCellContent = findPickupCellContent(registry, entity, drone, item, true);
				if (sourceCellContent == nullptr)
				{
					for (PickupTask* pickupTask : pickupTasks)
					{
						pickupTask->cancel(registry);
						delete pickupTask;
					}
					return false;
				}

				float amountToPickup = calculateAmountToPickup(registry, sourceCellContent, item, remainingAmountToPickup);
				remainingAmountToPickup -= amountToPickup;
				std::shared_ptr<world::IItem> itemAndAmount = item->clone(amountToPickup);

				if (lastCell == nullptr)
					lastCell = findNearestCell(registry, entity, sourceCellContent);
				else
					lastCell = findNearestCell(lastCell, sourceCellContent);
				pickupTasks.push_back(new PickupTask(lastCell, itemAndAmount, true, true));
			}
		}

		for (PickupTask* pickupTask : pickupTasks)
			drone.tasks.push(pickupTask);
		drone.tasks.push(new ConstructionTask(cellAndBuilding.first, cellAndBuilding.second));

		return true;
	}

	bool tryScheduleDestructionTask(entt::registry& registry, entt::entity& entity, world::Drone& drone, world::Cell* cell)
	{
		world::CellContent* destinationContent = cell->getContent();
		world::Inventory resourcesObtainedByRemoval = destinationContent->getResourcesObtainedByRemoval(cell);

		std::vector<DeliveryTask*> deliveryTasks;
		world::Cell* lastCell = cell;
		for (std::shared_ptr<world::IItem> item : resourcesObtainedByRemoval.items)
		{
			world::CellContent* destinationCellContent = findDeliveryCellContent(registry, entity, drone, item);
			if (destinationCellContent == nullptr)
			{
				for (DeliveryTask* deliveryTask : deliveryTasks)
				{
					deliveryTask->cancel(registry);
					delete deliveryTask;
				}
				return false;

				lastCell = findNearestCell(lastCell, destinationCellContent);
				deliveryTasks.push_back(new DeliveryTask(lastCell, item));
			}
		}

		drone.tasks.push(new DestructionTask(cell));
		for (DeliveryTask* deliveryTask : deliveryTasks)
			drone.tasks.push(deliveryTask);

		return true;
	}

	bool tryFindTaskToEmptyInventory(entt::registry& registry, entt::entity& entity, world::Drone& drone, world::Inventory& inventory)
	{
		for (std::shared_ptr<world::IItem> item : inventory.items)
		{
			world::CellContent* destinationCellContent = findDeliveryCellContent(registry, entity, drone, item);
			if (destinationCellContent != nullptr)
			{
				drone.tasks.push(new DeliveryTask(findNearestCell(registry, entity, destinationCellContent), item));
				return true;
			}
		}

		return false;
	}

	bool tryFindTaskForStarvingConsumer(entt::registry& registry, entt::entity& entity, world::Drone& drone)
	{
		while (!starvingConsumers.empty())
		{
			EntityAmount consumer = starvingConsumers.top();
			starvingConsumers.pop();

			world::CellContent* sourceCellContent = findPickupCellContent(registry, entity, drone, consumer.itemType, false);
			if (sourceCellContent != nullptr)
			{
				world::CellContent* destinationCellContent = registry.get<world::CellContentComponent>(consumer.entity).cellContent;
				schedulePickupAndDeliveryTask(registry, entity, drone, consumer.itemType, sourceCellContent, destinationCellContent);
				return true;
			}
		}

		return false;
	}

	bool tryFindConstructionTask(entt::registry& registry, entt::entity& entity, world::Drone& drone)
	{
		bool constructionTaskScheduled = false;
		std::vector<std::pair<world::Cell*, world::IBuilding*>> buildingsToReenqueue;

		while (!buildingsToPlace.empty())
		{
			std::pair<world::Cell*, world::IBuilding*> cellAndBuilding = buildingsToPlace.front();
			buildingsToPlace.pop();

			constructionTaskScheduled = tryScheduleConstructionTask(registry, entity, drone, cellAndBuilding);
			if (constructionTaskScheduled)
				break;
			else
				buildingsToReenqueue.push_back(cellAndBuilding);
		}

		for (std::pair<world::Cell*, world::IBuilding*>& cellAndBuilding : buildingsToReenqueue)
			buildingsToPlace.push(cellAndBuilding);
		
		return constructionTaskScheduled;
	}

	bool tryFindDestructionTask(entt::registry& registry, entt::entity& entity, world::Drone& drone)
	{
		bool destructionTaskScheduled = false;
		std::vector<world::Cell*> cellsToReenqueue;

		while (!buildingsToRemove.empty())
		{
			world::Cell* cell = buildingsToRemove.front();
			buildingsToRemove.pop();

			destructionTaskScheduled = tryScheduleDestructionTask(registry, entity, drone, cell);
			if (destructionTaskScheduled)
				break;
			else
				cellsToReenqueue.push_back(cell);
		}

		for (world::Cell* cell : cellsToReenqueue)
			buildingsToRemove.push(cell);

		return false;
	}

	bool tryFindTaskForFilledProducer(entt::registry& registry, entt::entity& entity, world::Drone& drone)
	{
		while (!filledProducers.empty())
		{
			EntityAmount producer = filledProducers.top();
			filledProducers.pop();

			world::CellContent* destinationCellContent = findDeliveryCellContent(registry, entity, drone, producer.itemType);
			if (destinationCellContent != nullptr)
			{
				world::CellContent* sourceCellContent = registry.get<world::CellContentComponent>(producer.entity).cellContent;
				schedulePickupAndDeliveryTask(registry, entity, drone, producer.itemType, sourceCellContent, destinationCellContent);

				return true;
			}
		}

		return false;
	}

	void tryFindTask(entt::registry& registry, entt::entity& entity, world::Drone& drone, world::Inventory& inventory) {
		if (tryFindTaskToEmptyInventory(registry, entity, drone, inventory))
			return;

		if (tryFindTaskForStarvingConsumer(registry, entity, drone))
			return;

		if (tryFindConstructionTask(registry, entity, drone))
			return;

		if (tryFindDestructionTask(registry, entity, drone))
			return;

		tryFindTaskForFilledProducer(registry, entity, drone);
	}

	void pursueTask(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		world::DroneTask* task,
		world::Inventory& inventory,
		rendering::components::EulerComponentwiseTransform& transform,
		double deltaTime
	) {
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

	void tryPursueTask(
		entt::registry& registry,
		entt::entity& entity,
		world::Drone& drone,
		world::Inventory& inventory,
		rendering::components::EulerComponentwiseTransform& transform,
		double deltaTime
	) {
		world::DroneTask* task = drone.tasks.front();
		if (task->checkAndUpdateDestination(registry, entity, drone, inventory))
		{
			// Task's destination is valid. Pursue the task by moving to the task's destination and performing the
			// task's action.
			pursueTask(registry, entity, drone, task, inventory, transform, deltaTime);
		}
		else
		{
			// The current task is no longer valid and can therefore be no longer pursued. Delete it from the
			// drone's tasks and continue with the next task on the next update.
			drone.tasks.pop();
			delete task;
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

		if (drone.tasks.empty())
			tryFindTask(registry, entity, drone, inventory);
		else
			tryPursueTask(registry, entity, drone, inventory, transform, deltaTime);

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

		filledProducers = std::priority_queue<EntityAmount, std::vector<EntityAmount>, MaxPriorityQueue>();
		starvingConsumers = std::priority_queue<EntityAmount, std::vector<EntityAmount>, MinPriorityQueue>();

		for (std::shared_ptr<world::IItem> itemType : itemTypes)
		{
			itemType->getConsumes()->iterateAllEntities(registry, std::function([&registry, itemType](entt::entity& entity, world::IConsumes* consumes) {
				float plannedAmount = registry.get<world::Inventory>(entity).getStoredAmount(itemType);
				auto& found = plannedInventoryChanges.find(entity);
				if (found != plannedInventoryChanges.end())
					plannedAmount += found->second.plannedDeliveries.getStoredAmount(itemType);
				
				if (plannedAmount <= world::RESOURCE_MANAGEMENT_RESUPPLY_CONSUMER_UNDER)
					starvingConsumers.push(EntityAmount{ entity, itemType, plannedAmount });
			}));

			itemType->getProduces()->iterateAllEntities(registry, std::function([&registry, itemType](entt::entity& entity, world::IProduces* produces) {
				float plannedAmount = registry.get<world::Inventory>(entity).getStoredAmount(itemType);
				auto& found = plannedInventoryChanges.find(entity);
				if (found != plannedInventoryChanges.end())
					plannedAmount -= found->second.plannedPickups.getStoredAmount(itemType);

				if (plannedAmount >= world::RESOURCE_MANAGEMENT_EMPTY_PRODUCER_ABOVE)
					filledProducers.push(EntityAmount{ entity, itemType, plannedAmount });
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

	void attachResourceProcessor(IResourceProcessor* resourceProcessor)
	{
		resourceProcessors.insert(resourceProcessor);
	}

	void registerItemType(std::shared_ptr<world::IItem> item)
	{
		itemTypes.insert(item);
	}
}
