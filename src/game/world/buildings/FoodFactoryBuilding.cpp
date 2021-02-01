#include "FoodFactoryBuilding.hpp"

namespace game::world
{
	static const std::string buildingTypeName = "Food Factory";
	static const std::string buildingDescription = "Consumes biomass to produce food.";
	static const Inventory constructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Stone>(4.0f), std::make_shared<Ores>(2.0f) });
	static const Inventory destructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Stone>(2.0f), std::make_shared<Ores>(1.0f) });

	static std::shared_ptr<BuildingPieceSet> pieceSet = std::make_shared<BuildingPieceSet>(
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Straight_Edge_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Straight_Edge_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Straight_Edge_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Straight_Edge_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Straight_Edge_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Inner_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Inner_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Inner_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Inner_Corner_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Inner_Corner_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Inner_Corner_Wall_Roof_Both"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Outer_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Outer_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/Outer_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) },
		std::vector<std::shared_ptr<NoEdgeBuildingPiece>>{ std::make_shared<NoEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Food Factory Building Piece Set/No_Edge_Roof"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
	) }
	);

	struct FoodFactoryBuildingComponent
	{
		FoodFactoryBuildingComponent(FoodFactoryBuilding* _building) : building(_building), lastProduced(glfwGetTime()) {}

		FoodFactoryBuilding* building;
		double lastProduced;
	};

	static class FoodFactoryResourceProcessor : public game::systems::IResourceProcessor {
		void processResources(entt::registry& registry, double deltaTime)
		{
			double time = glfwGetTime();

			for (auto& entity : registry.view<FoodFactoryBuildingComponent>())
			{
				registry.patch<FoodFactoryBuildingComponent>(entity, [&registry, entity, time](auto& building) {
					Inventory& inventory = registry.get<Inventory>(entity);
					std::shared_ptr<Biomass> biomass = inventory.getItemTyped<Biomass>();
					bool hasEnoughBiomass = biomass != nullptr && biomass->amount >= 2.0f;
					if (hasEnoughBiomass)
					{
						float occupiedSpace = building.building->getTotalAmountOfActualOccupiedSpace();
						float timeForProduction = 60.0f / occupiedSpace;

						if (time - building.lastProduced > timeForProduction)
						{
							building.lastProduced = time;
							inventory.removeItemTyped<Biomass>(2.0f);
							inventory.addItemTyped<Food>(1.0f);
						}
					}
					else
					{
						building.lastProduced = time;
					}
					});
			}
		}
	} resourceProcessor;

	FoodFactoryBuilding::FoodFactoryBuilding(
		IBuilding* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(buildingTypeName, buildingDescription, pieceSet, original, cellsToCopy) {}

	bool FoodFactoryBuilding::_canBePlacedOnCell(Cell* cell)
	{
		return true;
	}

	void FoodFactoryBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachResourceProcessor(&resourceProcessor);

		if (!getRegistry()->has<FoodFactoryBuildingComponent>(getEntity()))
		{
			getRegistry()->emplace<FoodFactoryBuildingComponent>(getEntity(), this);
			getRegistry()->emplace<Produces<Food>>(getEntity());
			getRegistry()->emplace<Consumes<Biomass>>(getEntity());
		}
	}

	void FoodFactoryBuilding::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void FoodFactoryBuilding::inventoryUpdated()
	{
		// Nothing to do here...
	}

	const Inventory& FoodFactoryBuilding::getResourcesRequiredToBuild()
	{
		return constructionResources;
	}

	const Inventory& FoodFactoryBuilding::getResourcesObtainedByRemoval()
	{
		return destructionResources;
	}
}
