#pragma once

#include "Building.hpp"
#include "../Resource.hpp"

namespace game::world
{
	class ReforesterBuilding : public Building<ReforesterBuilding>
	{
	public:
		ReforesterBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~ReforesterBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
