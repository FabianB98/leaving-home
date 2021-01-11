#include "Building.hpp"

namespace game::world
{
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

	static class : public game::systems::IResourceProcessor {
		void processResources(entt::registry& registry, double deltaTime)
		{
			double time = glfwGetTime();

			for (auto& entity : registry.view<TestBuildingComponent>())
			{
				registry.patch<TestBuildingComponent>(entity, [&registry, entity, time](auto& building) {
					if (time - building.lastConsumed > 5.0f)
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
	) : Building(testBuildingPieceSet, original, cellsToCopy) {}

	void TestBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachRessourceProcessor(&testBuildingResourceProcessor);

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

	OtherTestBuilding::OtherTestBuilding(
		IBuilding* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(testBuildingPieceSet, original, cellsToCopy) {}

	void OtherTestBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachRessourceProcessor(&testBuildingResourceProcessor);

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
}
