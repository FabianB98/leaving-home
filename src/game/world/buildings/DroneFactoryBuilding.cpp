#include "DroneFactoryBuilding.hpp"

namespace game::world
{
	static const std::string buildingTypeName = "Drone Factory";
	static const std::string buildingDescription = "A factory for producing more drones. Consumes ores for each drone to produce.";
	static const Inventory constructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Stone>(4.0f), std::make_shared<Ores>(2.0f) });
	static const Inventory destructionResources = Inventory(std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>{ std::make_shared<Stone>(2.0f), std::make_shared<Ores>(1.0f) });

	static std::shared_ptr<BuildingPieceSet> pieceSet = std::make_shared<BuildingPieceSet>(
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Straight_Edge_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Straight_Edge_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Straight_Edge_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Straight_Edge_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>{ std::make_shared<StraightEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Straight_Edge_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Inner_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Inner_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Inner_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Inner_Corner_Wall_Roof_Left"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Inner_Corner_Wall_Roof_Right"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>>{ std::make_shared<InnerCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Inner_Corner_Wall_Roof_Both"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Outer_Corner_Wall"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Outer_Corner_Wall_Roof_Outer_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>>{ std::make_shared<OuterCornerBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/Outer_Corner_Roof_Wall_Inner_Corner"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) },
		std::vector<std::shared_ptr<NoEdgeBuildingPiece>>{ std::make_shared<NoEdgeBuildingPiece>(
			"0",
			std::make_shared<rendering::model::MeshData>("Drone Hub Building Piece Set/No_Edge_Roof"),
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" },
			std::vector<std::string>{ "0" }
		) }
	);

	static class DroneFactoryResourceProcessor : public game::systems::IResourceProcessor {
		void processResources(entt::registry& registry, double deltaTime)
		{
			double time = glfwGetTime();

			for (auto& entity : registry.view<DroneFactoryBuildingComponent>())
			{
				registry.patch<DroneFactoryBuildingComponent>(entity, [&registry, entity, time](auto& building) {
					if (building.amountOfDronesToProduce > 0)
					{
						registry.emplace_or_replace<Consumes<Ores>>(entity);

						Inventory& inventory = registry.get<Inventory>(entity);
						std::shared_ptr<Ores> ores = inventory.getItemTyped<Ores>();
						bool hasEnoughOres = ores != nullptr && ores->amount >= 3.0f;
						if (hasEnoughOres)
						{
							float occupiedSpace = building.building->getTotalAmountOfActualOccupiedSpace();
							float timeForProduction = 30.0f + 30.0f / occupiedSpace;

							if (time - building.lastProduced > timeForProduction)
							{
								building.lastProduced = time;
								building.amountOfDronesToProduce -= 1;
								inventory.removeItemTyped<Ores>(3.0f);

								glm::vec3 cellPos = building.building->getCells().begin()->first->getRelaxedPositionAndHeight();
								Drone::spawnNewDrone(registry, cellPos + glm::vec3(0.0f, DRONE_FLIGHT_HEIGHT, 0.0f));
							}
						}
						else
						{
							building.lastProduced = time;
						}
					}
					else
					{
						building.lastProduced = time;
						registry.remove_if_exists<Consumes<Ores>>(entity);
					}
				});
			}
		}
	} resourceProcessor;

	DroneFactoryBuilding::DroneFactoryBuilding(
		IBuilding* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(buildingTypeName, buildingDescription, pieceSet, original, cellsToCopy) {}

	bool DroneFactoryBuilding::_canBePlacedOnCell(Cell* cell)
	{
		return true;
	}

	void DroneFactoryBuilding::__addedToCell(Cell* cell)
	{
		game::systems::attachResourceProcessor(&resourceProcessor);

		if (!getRegistry()->has<DroneFactoryBuildingComponent>(getEntity()))
			getRegistry()->emplace<DroneFactoryBuildingComponent>(getEntity(), this);
	}

	void DroneFactoryBuilding::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void DroneFactoryBuilding::inventoryUpdated()
	{
		// Nothing to do here...
	}

	const Inventory& DroneFactoryBuilding::getResourcesRequiredToBuild()
	{
		return constructionResources;
	}

	const Inventory& DroneFactoryBuilding::getResourcesObtainedByRemoval()
	{
		return destructionResources;
	}
}
