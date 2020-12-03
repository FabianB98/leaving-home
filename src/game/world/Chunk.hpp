#pragma once

#include "../../rendering/model/Material.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../../rendering/model/MeshPart.hpp"

#include <algorithm>
#include <random>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
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
		Chunk(size_t worldSeed, int32_t _column, int32_t _row);

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

		int32_t column;
		int32_t row;
		glm::vec2 centerPos;

		rendering::model::Mesh* mesh;

		class Generator
		{
		public:
			Generator(Chunk* _chunk) : chunk(_chunk), graph(PlanarGraph()) {}

			void generateChunkTopology();

		private:
			Chunk* chunk;

			PlanarGraph graph;
			std::vector<Node*> nodesOrdered;
			std::vector<std::pair<DirectedEdge*, DirectedEdge*>> edgesOrdered;

			size_t lineIndexPrefixsum[2 * CHUNK_SIZE + 1]{};

			void generateInitialPositions();

			void generateInitialEdges();

			void removeEdges();

			void subdivideSurfaces();

			void setChunkTopologyData();
		};
	};
}
