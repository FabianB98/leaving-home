#include "ReforesterBuilding.hpp"

namespace game::world
{
	static const std::string buildingTypeName = "Forest House";
	static const std::string buildingDescription = "Plants trees within its neighborhood, consuming biomass.";
	static const Inventory constructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Wood>(4.0f) });
	static const Inventory destructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Wood>(2.0f) });

	static std::shared_ptr<BuildingPieceSet> pieceSet = std::make_shared<BuildingPieceSet>(
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Straight_Edge_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Straight_Edge_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Straight_Edge_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Straight_Edge_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Straight_Edge_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Inner_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Inner_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Inner_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Inner_Corner_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Inner_Corner_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Inner_Corner_Wall_Roof_Both"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Outer_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Outer_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/Outer_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<NoEdgeBuildingPiece>>{ std::make_shared<NoEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Reforester Building Piece Set/No_Edge_Roof"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) }
	);

	struct ReforesterBuildingComponent
	{
		ReforesterBuildingComponent(ReforesterBuilding* _building) : building(_building), lastProduced(glfwGetTime()) {}

		ReforesterBuilding* building;
		double lastProduced;
	};

	static class ReforesterResourceProcessor : public game::systems::IResourceProcessor {
		void processResources(entt::registry& registry, double deltaTime)
		{
			double time = glfwGetTime();

			for (auto& entity : registry.view<ReforesterBuildingComponent>())
			{
				registry.patch<ReforesterBuildingComponent>(entity, [&registry, entity, time](auto& building) {
					float occupiedSpace = building.building->getTotalAmountOfActualOccupiedSpace();
					float requiredBiomass = 0.1f;

					Inventory& inventory = registry.get<Inventory>(entity);
					std::shared_ptr<Biomass> biomass = inventory.getItemTyped<Biomass>();
					bool hasEnoughBiomass = biomass != nullptr && biomass->amount >= requiredBiomass;

					if (hasEnoughBiomass)
					{
						float timeForProduction = 15.0f + 30.0f / occupiedSpace;
						if (time - building.lastProduced > timeForProduction)
						{
							building.lastProduced = time;

							auto& cells = building.building->getCells();
							std::random_device rd;
							std::mt19937 gen(rd());
							std::uniform_int_distribution<> distr(0, cells.size() - 1);
							auto& randomCell = std::next(std::begin(cells), distr(gen));

							Cell* treeCell = randomCell->first->getAnyNeighborFulfillingPredicate(10, std::function<bool(Cell*)>([](Cell* cell) {
								CellContent* cellContent = cell->getContent();
								return cellContent == nullptr && cell->getCellType() == CellType::GRASS;
								}));

							if (treeCell != nullptr)
							{
								treeCell->setContent(new Tree());

								inventory.removeItemTyped<Biomass>(requiredBiomass);
							}
						}
					}
					else {
						building.lastProduced = time;
					}
					});
			}
		}
	} resourceProcessor;

	ReforesterBuilding::ReforesterBuilding(
		IBuilding* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(buildingTypeName, buildingDescription, pieceSet, original, cellsToCopy) {}

	bool ReforesterBuilding::_canBePlacedOnCell(Cell* cell)
	{
		return true;
	}

	void ReforesterBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachResourceProcessor(&resourceProcessor);

		if (!getRegistry()->has<ReforesterBuildingComponent>(getEntity()))
		{
			getRegistry()->emplace<ReforesterBuildingComponent>(getEntity(), this);
			getRegistry()->emplace<Consumes<Biomass>>(getEntity());
		}
	}

	void ReforesterBuilding::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void ReforesterBuilding::inventoryUpdated()
	{
		// Nothing to do here...
	}

	const Inventory& ReforesterBuilding::getResourcesRequiredToBuild()
	{
		return constructionResources;
	}

	const Inventory& ReforesterBuilding::getResourcesObtainedByRemoval()
	{
		return destructionResources;
	}
}
