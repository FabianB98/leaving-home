#pragma once

#include <vector>

#include "Chunk.hpp"
#include "Inventory.hpp"

namespace game::world
{
	class BuildingPiece
	{
	public:
		BuildingPiece(
			std::string _id,
			std::shared_ptr<rendering::model::MeshData> _meshData,
			const std::vector<std::string>& _fitsLeft,
			const std::vector<std::string>& _fitsRight,
			const std::vector<std::string>& _fitsFront,
			const std::vector<std::string>& _fitsBack,
			const std::vector<std::string>& _fitsUp,
			const std::vector<std::string>& _fitsDown
		) : id(_id),
			meshData(_meshData),
			fitsLeft(_fitsLeft),
			fitsRight(_fitsRight),
			fitsFront(_fitsFront),
			fitsBack(_fitsBack),
			fitsUp(_fitsUp),
			fitsDown(_fitsDown) {}

		std::string getId()
		{
			return id;
		}

		const std::shared_ptr<rendering::model::MeshData> getMeshData()
		{
			return meshData;
		}

		const std::vector<std::string>& getFitsLeft()
		{
			return fitsLeft;
		}

		const std::vector<std::string>& getFitsRight()
		{
			return fitsRight;
		}

		const std::vector<std::string>& getFitsFront()
		{
			return fitsFront;
		}

		const std::vector<std::string>& getFitsBack()
		{
			return fitsBack;
		}

		const std::vector<std::string>& getFitsUp()
		{
			return fitsUp;
		}

		const std::vector<std::string>& getFitsDown()
		{
			return fitsDown;
		}

	private:
		std::string id;
		std::shared_ptr<rendering::model::MeshData> meshData;

		std::vector<std::string> fitsLeft;
		std::vector<std::string> fitsRight;
		std::vector<std::string> fitsFront;
		std::vector<std::string> fitsBack;
		std::vector<std::string> fitsUp;
		std::vector<std::string> fitsDown;
	};

	class StraightEdgeBuildingPiece : public BuildingPiece
	{
	public:
		StraightEdgeBuildingPiece(
			std::string _id,
			std::shared_ptr<rendering::model::MeshData> _meshData,
			const std::vector<std::string>& _fitsLeft,
			const std::vector<std::string>& _fitsRight,
			const std::vector<std::string>& _fitsUp,
			const std::vector<std::string>& _fitsDown
		) : BuildingPiece(
			_id,
			_meshData,
			_fitsLeft,
			_fitsRight,
			std::vector<std::string>(),
			std::vector<std::string>(),
			_fitsUp,
			_fitsDown
		) {}
	};

	class InnerCornerBuildingPiece : public BuildingPiece
	{
	public:
		InnerCornerBuildingPiece(
			std::string _id,
			std::shared_ptr<rendering::model::MeshData> _meshData,
			const std::vector<std::string>& _fitsLeft,
			const std::vector<std::string>& _fitsFront,
			const std::vector<std::string>& _fitsUp,
			const std::vector<std::string>& _fitsDown
		) : BuildingPiece(
			_id,
			_meshData,
			_fitsLeft,
			std::vector<std::string>(),
			_fitsFront,
			std::vector<std::string>(),
			_fitsUp,
			_fitsDown
		) {}
	};

	class OuterCornerBuildingPiece : public BuildingPiece
	{
	public:
		OuterCornerBuildingPiece(
			std::string _id,
			std::shared_ptr<rendering::model::MeshData> _meshData,
			const std::vector<std::string>& _fitsLeft,
			const std::vector<std::string>& _fitsFront,
			const std::vector<std::string>& _fitsUp,
			const std::vector<std::string>& _fitsDown
		) : BuildingPiece(
			_id,
			_meshData,
			_fitsLeft,
			std::vector<std::string>(),
			_fitsFront,
			std::vector<std::string>(),
			_fitsUp,
			_fitsDown
		) {}
	};

	class NoEdgeBuildingPiece : public BuildingPiece
	{
	public:
		NoEdgeBuildingPiece(
			std::string _id,
			std::shared_ptr<rendering::model::MeshData> _meshData,
			const std::vector<std::string>& _fitsLeft,
			const std::vector<std::string>& _fitsRight,
			const std::vector<std::string>& _fitsFront,
			const std::vector<std::string>& _fitsBack
		) : BuildingPiece(
			_id,
			_meshData,
			_fitsLeft,
			_fitsRight,
			_fitsFront,
			_fitsBack,
			std::vector<std::string>(),
			std::vector<std::string>()
		) {}
	};

	class BuildingPieceSet
	{
	public:
		BuildingPieceSet(
			const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& _straightEdgeWallPieces,
			const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& _straightEdgeWallRoofOuterCornerPieces,
			const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& _straightEdgeRoofWallInnerCornerPieces,
			const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& _straightEdgeWallRoofLeftPieces,
			const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& _straightEdgeWallRoofRightPieces,
			const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& _innerCornerWallPieces,
			const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& _innerCornerWallRoofOuterCornerPieces,
			const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& _innerCornerRoofWallInnerCornerPieces,
			const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& _innerCornerWallRoofLeftPieces,
			const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& _innerCornerWallRoofRightPieces,
			const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& _innerCornerWallRoofBothPieces,
			const std::vector<std::shared_ptr<OuterCornerBuildingPiece>>& _outerCornerWallPieces,
			const std::vector<std::shared_ptr<OuterCornerBuildingPiece>>& _outerCornerWallRoofOuterCornerPieces,
			const std::vector<std::shared_ptr<OuterCornerBuildingPiece>>& _outerCornerRoofWallInnerCornerPieces,
			const std::vector<std::shared_ptr<NoEdgeBuildingPiece>>& _noEdgeRoofPieces
		) : straightEdgeWallPieces(_straightEdgeWallPieces),
			straightEdgeWallRoofOuterCornerPieces(_straightEdgeWallRoofOuterCornerPieces),
			straightEdgeRoofWallInnerCornerPieces(_straightEdgeRoofWallInnerCornerPieces),
			straightEdgeWallRoofLeftPieces(_straightEdgeWallRoofLeftPieces),
			straightEdgeWallRoofRightPieces(_straightEdgeWallRoofRightPieces),
			innerCornerWallPieces(_innerCornerWallPieces),
			innerCornerWallRoofOuterCornerPieces(_innerCornerWallRoofOuterCornerPieces),
			innerCornerRoofWallInnerCornerPieces(_innerCornerRoofWallInnerCornerPieces),
			innerCornerWallRoofLeftPieces(_innerCornerWallRoofLeftPieces),
			innerCornerWallRoofRightPieces(_innerCornerWallRoofRightPieces),
			innerCornerWallRoofBothPieces(_innerCornerWallRoofBothPieces),
			outerCornerWallPieces(_outerCornerWallPieces),
			outerCornerWallRoofOuterCornerPieces(_outerCornerWallRoofOuterCornerPieces),
			outerCornerRoofWallInnerCornerPieces(_outerCornerRoofWallInnerCornerPieces),
			noEdgeRoofPieces(_noEdgeRoofPieces) {}

		const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& getStraightEdgeWallPieces()
		{
			return straightEdgeWallPieces;
		}

		const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& getStraightEdgeWallRoofOuterCornerPieces()
		{
			return straightEdgeWallRoofOuterCornerPieces;
		}

		const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& getStraightEdgeRoofWallInnerCornerPieces()
		{
			return straightEdgeRoofWallInnerCornerPieces;
		}

		const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& getStraightEdgeWallRoofLeftPieces()
		{
			return straightEdgeWallRoofLeftPieces;
		}

		const std::vector<std::shared_ptr<StraightEdgeBuildingPiece>>& getStraightEdgeWallRoofRightPieces()
		{
			return straightEdgeWallRoofRightPieces;
		}

		const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& getInnerCornerWallPieces()
		{
			return innerCornerWallPieces;
		}

		const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& getInnerCornerWallRoofOuterCornerPieces()
		{
			return innerCornerWallRoofOuterCornerPieces;
		}

		const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& getInnerCornerRoofWallInnerCornerPieces()
		{
			return innerCornerRoofWallInnerCornerPieces;
		}

		const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& getInnerCornerWallRoofLeftPieces()
		{
			return innerCornerWallRoofLeftPieces;
		}

		const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& getInnerCornerWallRoofRightPieces()
		{
			return innerCornerWallRoofRightPieces;
		}

		const std::vector<std::shared_ptr<InnerCornerBuildingPiece>>& getInnerCornerWallRoofBothPieces()
		{
			return innerCornerWallRoofBothPieces;
		}

		const std::vector<std::shared_ptr<OuterCornerBuildingPiece>>& getOuterCornerWallPieces()
		{
			return outerCornerWallPieces;
		}

		const std::vector<std::shared_ptr<OuterCornerBuildingPiece>>& getOuterCornerWallRoofOuterCornerPieces()
		{
			return outerCornerWallRoofOuterCornerPieces;
		}

		const std::vector<std::shared_ptr<OuterCornerBuildingPiece>>& getOuterCornerRoofWallInnerCornerPieces()
		{
			return outerCornerRoofWallInnerCornerPieces;
		}

		const std::vector<std::shared_ptr<NoEdgeBuildingPiece>>& getNoEdgeRoofPieces()
		{
			return noEdgeRoofPieces;
		}

	private:
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>> straightEdgeWallPieces;
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>> straightEdgeWallRoofOuterCornerPieces;
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>> straightEdgeRoofWallInnerCornerPieces;
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>> straightEdgeWallRoofLeftPieces;
		std::vector<std::shared_ptr<StraightEdgeBuildingPiece>> straightEdgeWallRoofRightPieces;

		std::vector<std::shared_ptr<InnerCornerBuildingPiece>> innerCornerWallPieces;
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>> innerCornerWallRoofOuterCornerPieces;
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>> innerCornerRoofWallInnerCornerPieces;
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>> innerCornerWallRoofLeftPieces;
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>> innerCornerWallRoofRightPieces;
		std::vector<std::shared_ptr<InnerCornerBuildingPiece>> innerCornerWallRoofBothPieces;

		std::vector<std::shared_ptr<OuterCornerBuildingPiece>> outerCornerWallPieces;
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>> outerCornerWallRoofOuterCornerPieces;
		std::vector<std::shared_ptr<OuterCornerBuildingPiece>> outerCornerRoofWallInnerCornerPieces;

		std::vector<std::shared_ptr<NoEdgeBuildingPiece>> noEdgeRoofPieces;
	};

	struct BuildingHeight
	{
		unsigned int plannedHeight;
		unsigned int actualHeight;

		unsigned int getMaxHeight()
		{
			return std::max(plannedHeight, actualHeight);
		}
	};

	class IBuilding : public CellContent
	{
	public:
		IBuilding(
			std::string& _typeName,
			std::string& _description,
			std::shared_ptr<BuildingPieceSet> _buildingPieceSet
		) : IBuilding(_typeName, _description, _buildingPieceSet, nullptr, std::unordered_set<Cell*>()) {}

		virtual ~IBuilding() {}

		virtual bool canBePlacedOnCell(Cell* cell) = 0;

		virtual void displayPlannedBuildingOfThisTypeOnCell(Cell* cell) = 0;

		virtual bool placeBuildingOfThisTypeOnCell(Cell* cell) = 0;

		virtual const Inventory& getResourcesRequiredToBuild() = 0;

		virtual const Inventory& getResourcesObtainedByRemoval() = 0;

		const Inventory _getResourcesObtainedByRemoval(Cell* cell)
		{
			const Inventory& resourcesPerHeight = getResourcesObtainedByRemoval();

			unsigned int height = 0;
			auto found = heightPerCell.find(cell);
			if (found != heightPerCell.end())
				height = found->second.actualHeight;

			Inventory result = Inventory();
			for (const std::shared_ptr<IItem>& item : resourcesPerHeight.items)
			{
				std::shared_ptr<IItem> clonedItem = item->clone();
				clonedItem->amount *= height;
				result.items.insert(clonedItem);
			}
			return result;
		}

		const std::unordered_map<Cell*, BuildingHeight>& getHeightPerCell()
		{
			return heightPerCell;
		}

		unsigned int getTotalAmountOfActualOccupiedSpace()
		{
			unsigned int result = 0;

			for (const std::pair<Cell*, BuildingHeight>& cellAndHeight : heightPerCell)
				result += cellAndHeight.second.actualHeight;

			return result;
		}

		unsigned int getTotalAmountOfPlannedOccupiedSpace()
		{
			unsigned int result = 0;

			for (const std::pair<Cell*, BuildingHeight>& cellAndHeight : heightPerCell)
				result += cellAndHeight.second.plannedHeight;

			return result;
		}

	protected:
		const std::shared_ptr<BuildingPieceSet> buildingPieceSet;
		std::unordered_map<Cell*, BuildingHeight> heightPerCell;

		IBuilding(
			const std::string& _typeName,
			const std::string& _description,
			std::shared_ptr<BuildingPieceSet> _buildingPieceSet,
			IBuilding* original,
			std::unordered_set<Cell*> cellsToCopy
		) : CellContent(true, _typeName, _description), buildingPieceSet(_buildingPieceSet)
		{
			if (original != nullptr)
			{
				for (Cell* cell : cellsToCopy)
				{
					BuildingHeight& originalHeight = original->heightPerCell[cell];
					BuildingHeight height{ originalHeight.plannedHeight - 1, originalHeight.actualHeight - 1 };
					heightPerCell.insert(std::make_pair(cell, height));
				}
			}
		}

		virtual CellContent* createNewCellContentOfSameType(std::unordered_set<Cell*> cellsToCopy) = 0;

		virtual void _enqueuedToAddToCell(Cell* cell) = 0;

		virtual void _addedToCell(Cell* cell) = 0;

		virtual void _enqueuedToRemoveFromCell(Cell* cell) = 0;

		virtual void _removedFromCell(Cell* cell) = 0;

		virtual void update() = 0;

		virtual void inventoryUpdated() = 0;
	};
}
