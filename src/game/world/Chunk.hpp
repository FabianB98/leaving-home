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

#include "PlanarGraph.hpp"

#define CHUNK_SIZE 5
#define CELL_SIZE 1

namespace game::world 
{
	class Cell;

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

		uint16_t getChunkId()
		{
			return chunkId;
		}

		int32_t getColumn()
		{
			return column;
		}

		int32_t getRow()
		{
			return row;
		}

		glm::vec2 getCenterPos()
		{
			return centerPos;
		}

		const std::vector<Cell*> getCells()
		{
			return cells;
		}

		Cell** getCellsAlongChunkBorder()
		{
			return cellsAlongChunkBorder;
		}

		rendering::model::Mesh* getMesh()
		{
			return mesh;
		}

	private:
		size_t chunkSeed;
		uint16_t chunkId;

		int32_t column;
		int32_t row;
		glm::vec2 centerPos;

		std::vector<Cell*> cells;
		Cell* cellsAlongChunkBorder[12 * CHUNK_SIZE];

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

	class Cell
	{
	public:
		Cell(Chunk* _chunk, uint16_t _cellId, glm::vec2 _position) : Cell(_chunk, _cellId, _position, std::vector<Cell*>()) {}

		Cell(Chunk* _chunk, uint16_t _cellId, glm::vec2 _position, std::vector<Cell*> _neighbors)
			: chunk(_chunk), cellId(_cellId), position(_position), neighbors(_neighbors)
		{
			completeId = (chunk->getChunkId() << 14) + cellId;
		}

		Chunk* getChunk()
		{
			return chunk;
		}

		uint16_t getCellId()
		{
			return cellId;
		}

		uint32_t getCompleteId()
		{
			return completeId;
		}

		glm::vec2 getPosition()
		{
			return position;
		}

		const std::vector<Cell*> getNeighbors()
		{
			return neighbors;
		}

	private:
		Chunk* chunk;

		uint16_t cellId;
		uint32_t completeId;

		glm::vec2 position;

		std::vector<Cell*> neighbors;
	};
}
