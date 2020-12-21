#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

#include "../../rendering/bounding_geometry/AABB.hpp"
#include "../../rendering/bounding_geometry/Sphere.hpp"
#include "../../rendering/components/Transform.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "Chunk.hpp"

namespace game::world
{
	class Resource : public CellContent
	{
	public:
		Resource(rendering::model::Mesh* _mesh, rendering::bounding_geometry::BoundingGeometry* _boundingGeometry) 
			: CellContent(_mesh, _boundingGeometry) {}

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
