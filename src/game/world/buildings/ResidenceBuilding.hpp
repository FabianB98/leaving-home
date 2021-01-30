#pragma once

#include "Building.hpp"

namespace game::world
{
	class ResidenceBuilding : public Building<ResidenceBuilding>
	{
	public:
		ResidenceBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~ResidenceBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
