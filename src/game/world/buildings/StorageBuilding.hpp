#pragma once

#include "Building.hpp"

namespace game::world
{
	class StorageBuilding : public Building<StorageBuilding>
	{
	public:
		StorageBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~StorageBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
