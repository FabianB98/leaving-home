#pragma once

#include "Building.hpp"

namespace game::world
{
	class TestBuilding : public Building<TestBuilding>
	{
	public:
		TestBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~TestBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};

	class OtherTestBuilding : public Building<OtherTestBuilding>
	{
	public:
		OtherTestBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~OtherTestBuilding() {}

		void inventoryUpdated();

		const Inventory& getResourcesRequiredToBuild();

		const Inventory& getResourcesObtainedByRemoval();

	protected:
		bool _canBePlacedOnCell(Cell* cell);

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
