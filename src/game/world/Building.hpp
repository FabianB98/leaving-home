#pragma once

#include <functional>
#include <stdexcept>
#include <vector>

#include <entt/entt.hpp>

#include "../../rendering/components/Transform.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../systems/ResourceProcessingSystem.hpp"
#include "BuildingPieceSet.hpp"
#include "Chunk.hpp"
#include "Constants.hpp"
#include "Item.hpp"

namespace game::world
{
	class IBuilding : public CellContent
	{
	public:
		IBuilding(std::shared_ptr<BuildingPieceSet> _buildingPieceSet)
			: IBuilding(_buildingPieceSet, nullptr, std::unordered_set<Cell*>()) {}

		virtual ~IBuilding() {}

		virtual void placeBuildingOfThisTypeOnCell(Cell* cell) = 0;

		const std::unordered_map<Cell*, unsigned int>& getHeightPerCell()
		{
			return heightPerCell;
		}

	protected:
		const std::shared_ptr<BuildingPieceSet> buildingPieceSet;
		std::unordered_map<Cell*, unsigned int> heightPerCell;

		IBuilding(
			std::shared_ptr<BuildingPieceSet> _buildingPieceSet,
			IBuilding* original,
			std::unordered_set<Cell*> cellsToCopy
		) : CellContent(true), buildingPieceSet(_buildingPieceSet)
		{
			if (original != nullptr)
				for (Cell* cell : cellsToCopy)
					heightPerCell.insert(std::make_pair(cell, original->heightPerCell[cell] - 1));
		}

		virtual CellContent* createNewCellContentOfSameType(std::unordered_set<Cell*> cellsToCopy) = 0;

		virtual void _addedToCell(Cell* cell) = 0;

		virtual void _removedFromCell(Cell* cell) = 0;

		virtual void update() = 0;
	};

	template <class BuildingType>
	class Building : public IBuilding
	{
	public:
		static BuildingType typeRepresentative;

		virtual ~Building() {}

		void placeBuildingOfThisTypeOnCell(Cell* cell)
		{
			cell->placeBuilding<BuildingType>();
		}

	protected:
		Building(
			std::shared_ptr<BuildingPieceSet> _buildingPieceSet,
			IBuilding* original,
			std::unordered_set<Cell*> cellsToCopy
		) : IBuilding(_buildingPieceSet, original, cellsToCopy) {}

		CellContent* createNewCellContentOfSameType(std::unordered_set<Cell*> cellsToCopy)
		{
			return new BuildingType(this, cellsToCopy);
		}

		void _addedToCell(Cell* cell)
		{
			auto& height = heightPerCell.find(cell);
			if (height != heightPerCell.end())
				height->second += 1;
			else
				heightPerCell.insert(std::make_pair(cell, 1));

			enqueueUpdate();

			__addedToCell(cell);
		}

		virtual void __addedToCell(Cell* cell) = 0;

		void _removedFromCell(Cell* cell)
		{
			heightPerCell.erase(cell);

			if (!heightPerCell.empty())
				enqueueUpdate();

			__removedFromCell(cell);
		}

		virtual void __removedFromCell(Cell* cell) = 0;

		void update()
		{
			rebuildMeshData();
		}

	private:
		void rebuildMeshData()
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

		void addMeshPieces(
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

		std::shared_ptr<rendering::model::MeshData> constructMeshDataForPiece(
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

		glm::vec2 interpolateBilinear(
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

		bool occupies(Cell* cell, unsigned int floor)
		{
			if (cell->getContent() != this)
				return false;

			auto& found = heightPerCell.find(cell);
			return found != heightPerCell.end() && found->second > floor;
		}
	};

	template <class BuildingType>
	BuildingType Building<BuildingType>::typeRepresentative = BuildingType(nullptr, std::unordered_set<Cell*>{});

	class TestBuilding : public Building<TestBuilding>
	{
	public:
		TestBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~TestBuilding() {}

	protected:
		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};

	class OtherTestBuilding : public Building<OtherTestBuilding>
	{
	public:
		OtherTestBuilding(IBuilding* original, std::unordered_set<Cell*> cellsToCopy);

		virtual ~OtherTestBuilding() {}

	protected:
		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);
	};
}
