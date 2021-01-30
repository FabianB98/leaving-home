#pragma once

#include "Building.hpp"

namespace game::world
{
	class MineBuilding : public Building<MineBuilding>
	{
	public:
		MineBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~MineBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
