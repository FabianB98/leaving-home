#pragma once

#include "../../rendering/model/Material.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../../rendering/model/MeshPart.hpp"

#include <stdlib.h>
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Cell.hpp"
#include "PlanarGraph.hpp"

#define CHUNK_SIZE 5

namespace game::world 
{
	class Chunk
	{
	public:
		Chunk(glm::vec2 _centerPos)
			: centerPos(_centerPos), mesh(generateMesh()) {};

		~Chunk()
		{
			delete mesh;
		}

		glm::vec2 getCenterPos()
		{
			return centerPos;
		}

		rendering::model::Mesh* getMesh()
		{
			return mesh;
		}

	private:
		glm::vec2 centerPos;

		rendering::model::Mesh* mesh;

		rendering::model::Mesh* generateMesh();
	};
}
