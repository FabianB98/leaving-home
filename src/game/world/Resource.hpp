#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

#include "../../rendering/components/Transform.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "Chunk.hpp"

namespace game::world
{
	class Resource : public CellContent
	{
	public:
		Resource(std::shared_ptr<rendering::model::MeshData> _meshData) : CellContent(false), meshData(_meshData) {}

	protected:
		void addedToCell(Cell* cell);

		void removedFromCell(Cell* cell);

		virtual void _addedToCell(Cell* cell) = 0;

		virtual void _removedFromCell(Cell* cell) = 0;

	private:
		std::shared_ptr<rendering::model::MeshData> meshData;
	};

	class Tree : public Resource
	{
	public:
		Tree();

	protected:
		void _addedToCell(Cell* cell);

		void _removedFromCell(Cell* cell);
	};
}
