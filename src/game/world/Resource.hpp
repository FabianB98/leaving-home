#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

#include "../../rendering/components/Transform.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "Chunk.hpp"
#include "Item.hpp"

namespace game::world
{
	class Resource : public CellContent
	{
	public:
		Resource(
			const std::string& _typeName,
			const std::string& _description,
			std::shared_ptr<rendering::model::MeshData> _meshData
		) : CellContent(false, _typeName, _description), meshData(_meshData) {}

		void inventoryUpdated();

		const Inventory _getResourcesObtainedByRemoval(Cell* cell);

	protected:
		virtual CellContent* createNewCellContentOfSameType(std::unordered_set<Cell*> cellsToCopy) = 0;

		void _addedToCell(Cell* cell);

		void _removedFromCell(Cell* cell);

		virtual void __addedToCell(Cell* cell) = 0;

		virtual void __removedFromCell(Cell* cell) = 0;

		virtual void update() = 0;

	private:
		std::shared_ptr<rendering::model::MeshData> meshData;
	};

	class Tree : public Resource
	{
	public:
		Tree();

	protected:
		CellContent* createNewCellContentOfSameType(std::unordered_set<Cell*> cellsToCopy)
		{
			return new Tree();
		}

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);

		void update();
	};

	class Rock : public Resource
	{
	public:
		Rock();

	protected:
		CellContent* createNewCellContentOfSameType(std::unordered_set<Cell*> cellsToCopy)
		{
			return new Rock();
		}

		void __addedToCell(Cell* cell);

		void __removedFromCell(Cell* cell);

		void update();
	};
}
