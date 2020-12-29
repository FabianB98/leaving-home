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
		Resource(std::shared_ptr<rendering::model::MeshData> _meshData) : CellContent(_meshData) {}

	protected:
		virtual void addedToCell() = 0;
	};

	class Tree : public Resource
	{
	public:
		Tree();

	protected:
		void addedToCell();
	};
}
