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

	static class : public game::systems::IResourceProcessor {
		void processResources(entt::registry& registry, double deltaTime)
		{
			double time = glfwGetTime();

			auto view = registry.view<TestBuildingComponent>();
			for (auto& entity : view)
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
		}
	} testBuildingResourceProcessor;

	Building::Building(
		std::shared_ptr<BuildingPieceSet> _buildingPieceSet,
		Building* original,
		std::unordered_set<Cell*> cellsToCopy
	) : CellContent(true), buildingPieceSet(_buildingPieceSet)
	{
		if (original != nullptr)
			for (Cell* cell : cellsToCopy)
				heightPerCell.insert(std::make_pair(cell, original->heightPerCell[cell] - 1));
	}

	void Building::_addedToCell(Cell* cell)
	{
		auto& height = heightPerCell.find(cell);
		if (height != heightPerCell.end())
			height->second += 1;
		else
			heightPerCell.insert(std::make_pair(cell, 1));

		enqueueUpdate();

		__addedToCell(cell);
	}

	void Building::_removedFromCell(Cell* cell)
	{
		heightPerCell.erase(cell);

		if (!heightPerCell.empty())
			enqueueUpdate();

		__removedFromCell(cell);
	}

	void Building::update()
	{
		rebuildMeshData();
	}

	void Building::rebuildMeshData()
	{
		std::unordered_map<Cell*, std::vector<std::shared_ptr<rendering::model::MeshData>>> meshPieces;
		for (auto& cellAndHeight : heightPerCell)
			for (auto& face : cellAndHeight.first->getFaces())
				for (unsigned int floor = 0; floor < cellAndHeight.second; floor++)
					addMeshPieces(meshPieces, cellAndHeight.first, face, floor);
		
		for (auto& cellAndPieces : meshPieces)
		{
			std::vector<std::pair<std::shared_ptr<rendering::model::MeshData>, std::vector<rendering::model::MeshDataInstance>>> instances;
			for (auto piece : cellAndPieces.second)
			{
				auto instance = rendering::model::MeshDataInstance(glm::mat4(1.0f));
				instances.push_back(std::make_pair(piece, std::vector<rendering::model::MeshDataInstance> { instance }));
			}

			setMeshData(cellAndPieces.first, std::make_shared<rendering::model::MeshData>(instances));
		}
	}

	void Building::addMeshPieces(
		std::unordered_map<Cell*, std::vector<std::shared_ptr<rendering::model::MeshData>>>& meshPieces,
		Cell* cell,
		Face* face,
		unsigned int floor
	) {
		// Determine the index of the cell within the face.
		unsigned int cellIndex = 0;
		auto& nodes = face->getNodes();
		while ((Cell*)(nodes[cellIndex]->getAdditionalData()) != cell)
			cellIndex++;

		// Get pointers to the other three cells of the face.
		Cell* counterClockwiseNeighborCell = (Cell*)(nodes[(cellIndex + 1) % 4]->getAdditionalData());
		Cell* diagonalNeighborCell = (Cell*)(nodes[(cellIndex + 2) % 4]->getAdditionalData());
		Cell* clockwiseNeighborCell = (Cell*)(nodes[(cellIndex + 3) % 4]->getAdditionalData());

		// Determine which corners of the current cube are occupied.
		bool occupiedCellUp = occupies(cell, floor + 1);
		bool occupiedCounterClockwiseDown = occupies(counterClockwiseNeighborCell, floor);
		bool occupiedCounterClockwiseUp = occupies(counterClockwiseNeighborCell, floor + 1);
		bool occupiedDiagonalDown = occupies(diagonalNeighborCell, floor);
		bool occupiedDiagonalUp = occupies(diagonalNeighborCell, floor + 1);
		bool occupiedClockwiseDown = occupies(clockwiseNeighborCell, floor);
		bool occupiedClockwiseUp = occupies(clockwiseNeighborCell, floor + 1);

		// Determine whether we must place a straight edge piece, an outer corner piece, an inner corner piece or a
		// no edge piece.
		bool straightEdgeDown = occupiedClockwiseDown != occupiedCounterClockwiseDown;
		bool straightEdgeUp = occupiedClockwiseUp != occupiedCounterClockwiseUp;
		bool outerCornerDown = !occupiedClockwiseDown && !occupiedCounterClockwiseDown;
		bool outerCornerUp = !occupiedClockwiseUp && !occupiedCounterClockwiseUp;
		bool innerCornerDown = occupiedClockwiseDown && occupiedCounterClockwiseDown && !occupiedDiagonalDown;
		bool innerCornerUp = occupiedClockwiseUp && occupiedCounterClockwiseUp && !occupiedDiagonalUp;
		bool noEdgeDown = occupiedClockwiseDown && occupiedCounterClockwiseDown && occupiedDiagonalDown;
		bool noEdgeUp = occupiedClockwiseUp && occupiedCounterClockwiseUp && occupiedDiagonalUp;

		// Calculate the height at which the pieces must be located at.
		float lowerHeight = cell->getHeight() + floor * BUILDING_FLOOR_HEIGHT;
		float centerHeight = cell->getHeight() + (floor + 0.5f) * BUILDING_FLOOR_HEIGHT;
		float upperHeight = cell->getHeight() + (floor + 1) * BUILDING_FLOOR_HEIGHT;

		// Calculate the position of the face's center.
		glm::vec2 faceCenterPos = (cell->getRelaxedPosition() + counterClockwiseNeighborCell->getRelaxedPosition()
				+ diagonalNeighborCell->getRelaxedPosition() + clockwiseNeighborCell->getRelaxedPosition()) / 4.0f;

		// Determine the positions of the piece's four corners (from a top-down view).
		glm::vec2 frontLeft, frontRight, backLeft, backRight;
		bool onRightHalf = straightEdgeDown && occupiedClockwiseDown || !straightEdgeDown && !noEdgeDown
			|| noEdgeDown && (straightEdgeUp && occupiedClockwiseUp || !straightEdgeUp);
		if (onRightHalf)
		{
			frontLeft = faceCenterPos;
			frontRight = (cell->getRelaxedPosition() + counterClockwiseNeighborCell->getRelaxedPosition()) / 2.0f;
			backLeft = (cell->getRelaxedPosition() + clockwiseNeighborCell->getRelaxedPosition()) / 2.0f;
			backRight = cell->getRelaxedPosition();
		}
		else
		{
			frontLeft = (cell->getRelaxedPosition() + clockwiseNeighborCell->getRelaxedPosition()) / 2.0f;
			frontRight = faceCenterPos;
			backLeft = cell->getRelaxedPosition();
			backRight = (cell->getRelaxedPosition() + counterClockwiseNeighborCell->getRelaxedPosition()) / 2.0f;
		}

		// Determine which pieces need to be placed. Beware: Ugly tree of nested if-else-branches ahead!
		std::shared_ptr<BuildingPiece> lowerPiece;
		std::shared_ptr<BuildingPiece> upperPiece;
		if (straightEdgeDown)
		{
			// TODO: As of now, only the first building piece of each type will be used. Once we have buildings which
			// have multiple pieces per type, some algorithm (for example Wave Function Collapse) should be used to
			// determine the actual piece to place.
			lowerPiece = buildingPieceSet->getStraightEdgeWallPieces()[0];
			if (occupiedCellUp)
			{
				if (outerCornerUp && onRightHalf)
				{
					upperPiece = buildingPieceSet->getStraightEdgeWallRoofRightPieces()[0];
				}
				else if (outerCornerUp && !onRightHalf)
				{
					upperPiece = buildingPieceSet->getStraightEdgeWallRoofLeftPieces()[0];
				}
				else 
				{
					upperPiece = buildingPieceSet->getStraightEdgeWallPieces()[0];
				}
			}
			else
			{
				upperPiece = buildingPieceSet->getStraightEdgeWallRoofOuterCornerPieces()[0];
			}
		}
		else if (outerCornerDown)
		{
			lowerPiece = buildingPieceSet->getOuterCornerWallPieces()[0];
			if (occupiedCellUp)
			{
				upperPiece = buildingPieceSet->getOuterCornerWallPieces()[0];
			}
			else
			{
				upperPiece = buildingPieceSet->getOuterCornerWallRoofOuterCornerPieces()[0];
			}
		}
		else if (innerCornerDown)
		{
			lowerPiece = buildingPieceSet->getInnerCornerWallPieces()[0];
			if (occupiedCellUp)
			{
				if (straightEdgeUp && occupiedCounterClockwiseUp)
				{
					upperPiece = buildingPieceSet->getInnerCornerWallRoofRightPieces()[0];
				}
				else if (straightEdgeUp && occupiedClockwiseUp)
				{
					upperPiece = buildingPieceSet->getInnerCornerWallRoofLeftPieces()[0];
				}
				else if (outerCornerUp)
				{
					upperPiece = buildingPieceSet->getInnerCornerWallRoofBothPieces()[0];
				}
				else
				{
					upperPiece = buildingPieceSet->getInnerCornerWallPieces()[0];
				}
			}
			else
			{
				upperPiece = buildingPieceSet->getInnerCornerWallRoofOuterCornerPieces()[0];
			}
		}
		else if (noEdgeDown)
		{
			lowerPiece = nullptr;
			if (!occupiedCellUp)
			{
				upperPiece = buildingPieceSet->getNoEdgeRoofPieces()[0];
			}
			else
			{
				if (straightEdgeUp)
				{
					upperPiece = buildingPieceSet->getStraightEdgeRoofWallInnerCornerPieces()[0];
				}
				else if (outerCornerUp)
				{
					upperPiece = buildingPieceSet->getOuterCornerRoofWallInnerCornerPieces()[0];
				}
				else if (innerCornerUp)
				{
					upperPiece = buildingPieceSet->getInnerCornerRoofWallInnerCornerPieces()[0];
				}
				else if (noEdgeUp)
				{
					upperPiece = nullptr;
				}
				else
				{
					throw std::logic_error("Piece is neither an edge nor a corner nor an inner piece! This must be a bug.");
				}
			}
				
		}
		else
		{
			throw std::logic_error("Piece is neither an edge nor a corner nor an inner piece! This must be a bug.");
		}

		if (lowerPiece != nullptr)
		{
			auto data = constructMeshDataForPiece(lowerPiece, frontLeft, frontRight, backLeft, backRight, lowerHeight, centerHeight);
			meshPieces[cell].push_back(data);
		}

		if (upperPiece != nullptr)
		{
			auto data = constructMeshDataForPiece(upperPiece, frontLeft, frontRight, backLeft, backRight, centerHeight, upperHeight);
			meshPieces[cell].push_back(data);
		}
	}

	std::shared_ptr<rendering::model::MeshData> Building::constructMeshDataForPiece(
		std::shared_ptr<BuildingPiece> piece,
		glm::vec2 frontLeft,
		glm::vec2 frontRight,
		glm::vec2 backLeft,
		glm::vec2 backRight,
		float lowerHeight,
		float upperHeight
	) {
		float deltaHeight = upperHeight - lowerHeight;
		constexpr float normalEpsilon = 0.1f;

		auto meshData = std::make_shared<rendering::model::MeshData>(*piece->getMeshData());
		for (int i = 0; i < meshData->vertices.size(); i++)
		{
			// Transform the vertex from object coordinates to world coordinates.
			glm::vec3& vertex = meshData->vertices[i];
			glm::vec2 pos = glm::vec2(vertex.x, vertex.z);
			glm::vec2 transformedPos = interpolateBilinear(pos, frontLeft, frontRight, backLeft, backRight);
			vertex = glm::vec3(transformedPos.x, vertex.y * deltaHeight + lowerHeight, transformedPos.y);

			// Transform the corresponding normal from object coordinates to world coordinates.
			glm::vec3& normal = meshData->normals[i];
			pos += normalEpsilon * glm::vec2(normal.x, normal.z);
			glm::vec2 normalDir = interpolateBilinear(pos, frontLeft, frontRight, backLeft, backRight) - transformedPos;
			normal = glm::normalize(glm::vec3(normalDir.x, normalEpsilon * normal.y * deltaHeight, normalDir.y));
		}

		return meshData;
	}

	glm::vec2 Building::interpolateBilinear(
		const glm::vec2& point,
		const glm::vec2& frontLeft,
		const glm::vec2& frontRight,
		const glm::vec2& backLeft,
		const glm::vec2& backRight
	) {
		return (1 - point.x) * (1 - point.y) * frontLeft
			+ point.x * (1 - point.y) * frontRight
			+ (1 - point.x) * point.y * backLeft
			+ point.x * point.y * backRight;
	}

	bool Building::occupies(Cell* cell, unsigned int floor)
	{
		if (cell->getContent() != this)
			return false;

		auto& found = heightPerCell.find(cell);
		return found != heightPerCell.end() && found->second > floor;
	}

	TestBuilding::TestBuilding() : Building(testBuildingPieceSet) 
	{
		game::systems::attachRessourceProcessor(&testBuildingResourceProcessor);
	}

	TestBuilding::TestBuilding(
		Building* original,
		std::unordered_set<Cell*> cellsToCopy
	) : Building(testBuildingPieceSet, original, cellsToCopy) {}

	void TestBuilding::__addedToCell(Cell* cell)
	{
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
}
