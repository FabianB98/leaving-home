#pragma once

#include "Building.hpp"
#include "../Resource.hpp"

namespace game::world
{
	class WoodcutterBuilding : public Building<WoodcutterBuilding>
	{
	public:
		WoodcutterBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~WoodcutterBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
