#pragma once

#include "../../rendering/model/Material.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../../rendering/model/MeshPart.hpp"

#include <algorithm>
#include <random>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "Cell.hpp"
#include "PlanarGraph.hpp"

#define CHUNK_SIZE 5

namespace game::world 
{
	class Chunk
	{
	public:
		Chunk(size_t worldSeed, glm::vec2 _centerPos)
			: chunkSeed(worldSeed ^ std::hash<glm::vec2>()(_centerPos)), centerPos(_centerPos) 
		{
			generateMesh();
		}

		~Chunk()
		{
			delete mesh;
		}

		size_t getChunkSeed()
		{
			return chunkSeed;
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
		size_t chunkSeed;
		glm::vec2 centerPos;

		rendering::model::Mesh* mesh;

		void generateMesh();
	};
}
