#pragma once

#include "Building.hpp"
#include "../Constants.hpp"
#include "../Drone.hpp"

namespace game::world
{
	class DroneFactoryBuilding : public Building<DroneFactoryBuilding>
	{
	public:
		DroneFactoryBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~DroneFactoryBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};

	struct DroneFactoryBuildingComponent
	{
		DroneFactoryBuildingComponent(DroneFactoryBuilding* _building) : building(_building), lastProduced(glfwGetTime()), amountOfDronesToProduce(1) {}

		DroneFactoryBuilding* building;
		double lastProduced;
		unsigned int amountOfDronesToProduce;
	};
}
