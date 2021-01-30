#include "TestBuildings.hpp"

namespace game::world
{
	static const std::string testBuildingTypeName = "TestBuilding";
	static const std::string testBuildingDescription = "A building which consumes one unit of wood each 15 seconds. If it doesn't have any wood to consume, it will get destroyed over time.";
	static const Inventory testBuildingConstructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Wood>(5.0f) });
	static const Inventory testBuildingDestructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Wood>(2.5f) });

	static const std::string otherTestBuildingTypeName = "OtherTestBuilding";
	static const std::string otherTestBuildingDescription = "A building which produces one unit of wood each 5 seconds.";
	static const Inventory otherTestBuildingConstructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Wood>(4.0f) });
	static const Inventory otherTestBuildingDestructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Wood>(2.0f) });

	static std::shared_ptr<BuildingPieceSet> testBuildingPieceSet = std::make_shared<BuildingPieceSet>(
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Straight_Edge_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Straight_Edge_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Straight_Edge_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Straight_Edge_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Straight_Edge_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Inner_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Inner_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Inner_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Inner_Corner_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Inner_Corner_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Inner_Corner_Wall_Roof_Both"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Outer_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Outer_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/Outer_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<NoEdgeBuildingPiece>>{ std::make_shared<NoEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Test Building Piece Set/No_Edge_Roof"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) }
	);

	struct TestBuildingComponent
	{
		TestBuildingComponent(TestBuilding* _building) : building(_building), lastConsumed(glfwGetTime()) {}

		TestBuilding* building;
		double lastConsumed;
	};

	struct OtherTestBuildingComponent
	{
		OtherTestBuildingComponent(OtherTestBuilding* _building) : building(_building), lastProduced(glfwGetTime()) {}

		OtherTestBuilding* building;
		double lastProduced;
	};

	static class TestBuildingsResourceProcessor : public game::systems::IResourceProcessor {
		void processResources(entt::registry& registry, double deltaTime)
		{
			double time = glfwGetTime();

			for (auto& entity : registry.view<TestBuildingComponent>())
			{
				registry.patch<TestBuildingComponent>(entity, [&registry, entity, time](auto& building) {
					if (time - building.lastConsumed > 15.0f)
					{
						building.lastConsumed = time;

						std::shared_ptr<Wood> wood = registry.get<Inventory>(entity).getItemTyped<Wood>();
						bool removeBuilding = true;
						if (wood != nullptr && wood->amount >= 1.0f)
						{
							removeBuilding = false;
							wood->amount -= 1.0f;
						}

						if (removeBuilding)
							building.building->getCells().begin()->first->setContent(nullptr);
					}
				});
			}

			for (auto& entity : registry.view<OtherTestBuildingComponent>())
			{
				registry.patch<OtherTestBuildingComponent>(entity, [&registry, entity, time](auto& building) {
					if (time - building.lastProduced > 5.0f)
					{
						building.lastProduced = time;

						registry.get<Inventory>(entity).addItemTyped<Wood>(1.0f);
					}
				});
			}
		}
	} testBuildingResourceProcessor;

	TestBuilding::TestBuilding(
		IBuilding* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(testBuildingTypeName, testBuildingDescription, testBuildingPieceSet, original, cellsToCopy) {}

	bool TestBuilding::_canBePlacedOnCell(Cell* cell)
	{
		return true;
	}

	void TestBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachResourceProcessor(&testBuildingResourceProcessor);

		if (!getRegistry()->has<TestBuildingComponent>(getEntity()))
		{
			getRegistry()->emplace<TestBuildingComponent>(getEntity(), this);
			getRegistry()->emplace<Consumes<Wood>>(getEntity());
		}
	}

	void TestBuilding::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void TestBuilding::inventoryUpdated()
	{
		// Nothing to do here...
	}

	const Inventory& TestBuilding::getResourcesRequiredToBuild()
	{
		return testBuildingConstructionResources;
	}

	const Inventory& TestBuilding::getResourcesObtainedByRemoval()
	{
		return testBuildingDestructionResources;
	}

	OtherTestBuilding::OtherTestBuilding(
		IBuilding* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(otherTestBuildingTypeName, otherTestBuildingDescription, testBuildingPieceSet, original, cellsToCopy) {}

	bool OtherTestBuilding::_canBePlacedOnCell(Cell* cell)
	{
		return true;
	}

	void OtherTestBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachResourceProcessor(&testBuildingResourceProcessor);

		if (!getRegistry()->has<OtherTestBuildingComponent>(getEntity()))
		{
			getRegistry()->emplace<OtherTestBuildingComponent>(getEntity(), this);
			getRegistry()->emplace<Produces<Wood>>(getEntity());
		}
	}

	void OtherTestBuilding::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void OtherTestBuilding::inventoryUpdated()
	{
		// Nothing to do here...
	}

	const Inventory& OtherTestBuilding::getResourcesRequiredToBuild()
	{
		return otherTestBuildingConstructionResources;
	}

	const Inventory& OtherTestBuilding::getResourcesObtainedByRemoval()
	{
		return otherTestBuildingDestructionResources;
	}
}
