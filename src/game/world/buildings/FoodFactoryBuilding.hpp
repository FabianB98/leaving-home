#pragma once

#include "Building.hpp"

namespace game::world
{
	class FoodFactoryBuilding : public Building<FoodFactoryBuilding>
	{
	public:
		FoodFactoryBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~FoodFactoryBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
