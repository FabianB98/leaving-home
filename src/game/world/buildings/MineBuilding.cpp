#include "MineBuilding.hpp"

namespace game::world
{
	static const std::string buildingTypeName = "Mine Building";
	static const std::string buildingDescription = "A mine. Mines stone and ore from deep under the ground.";
	static const Inventory constructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Stone>(4.0f) });
	static const Inventory destructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Stone>(2.0f) });

	static std::shared_ptr<BuildingPieceSet> pieceSet = std::make_shared<BuildingPieceSet>(
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Straight_Edge_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Straight_Edge_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Straight_Edge_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Straight_Edge_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Straight_Edge_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Inner_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Inner_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Inner_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Inner_Corner_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Inner_Corner_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Inner_Corner_Wall_Roof_Both"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Outer_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Outer_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/Outer_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<NoEdgeBuildingPiece>>{ std::make_shared<NoEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Mine Building Piece Set/No_Edge_Roof"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) }
	);

	struct MineBuildingComponent
	{
		MineBuildingComponent(MineBuilding* _building) : building(_building), lastProduced(glfwGetTime()) {}

		MineBuilding* building;
		double lastProduced;
	};

	static class : public game::systems::IResourceProcessor {
		void processResources(entt::registry& registry, double deltaTime)
		{
			double time = glfwGetTime();

			for (auto& entity : registry.view<MineBuildingComponent>())
			{
				registry.patch<MineBuildingComponent>(entity, [&registry, entity, time](auto& building) {
					if (time - building.lastProduced > 30.0f)
					{
						building.lastProduced = time;

						float occupiedSpace = building.building->getTotalAmountOfActualOccupiedSpace();
						registry.get<Inventory>(entity).addItemTyped<Stone>(1.0f * occupiedSpace);
						registry.get<Inventory>(entity).addItemTyped<Ores>(0.5f * occupiedSpace);
					}
				});
			}
		}
	} resourceProcessor;

	MineBuilding::MineBuilding(
		IBuilding* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(buildingTypeName, buildingDescription, pieceSet, original, cellsToCopy) {}

	bool MineBuilding::_canBePlacedOnCell(Cell* cell)
	{
		return cell->getCellType() == CellType::STONE;
	}

	void MineBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachResourceProcessor(&resourceProcessor);

		if (!getRegistry()->has<MineBuildingComponent>(getEntity()))
		{
			getRegistry()->emplace<MineBuildingComponent>(getEntity(), this);
			getRegistry()->emplace<Produces<Stone>>(getEntity());
			getRegistry()->emplace<Produces<Ores>>(getEntity());
		}
	}

	void MineBuilding::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void MineBuilding::inventoryUpdated()
	{
		// Nothing to do here...
	}

	const Inventory& MineBuilding::getResourcesRequiredToBuild()
	{
		return constructionResources;
	}

	const Inventory& MineBuilding::getResourcesObtainedByRemoval()
	{
		return destructionResources;
	}
}
