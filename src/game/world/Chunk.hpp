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
		Chunk(
			size_t worldSeed,
			int32_t _column,
			int32_t _row,
			Chunk* _neighbors[6],
			PlanarGraph* _worldGraph
		) : Chunk(
			worldSeed,
			_column,
			_row,
			_neighbors,
			_worldGraph,
			CHUNK_SIZE,
			CELL_SIZE
		) {};

		~Chunk();

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

		const std::vector<Cell*> getCellsAlongChunkBorder()
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
		std::vector<Cell*> cellsAlongChunkBorder;

		rendering::model::Mesh* mesh;

		const int chunkSize;
		const float cellSize;

		const int initialCellSize;
		const int chunkBorderLength;
		const int totalBorderLength;

		const float chunkWidth;
		const float chunkHeight;

		const float chunkHorizontalDistance;
		const float chunkVerticalDistance;

		Chunk(
			size_t worldSeed,
			int32_t _column,
			int32_t _row,
			Chunk* _neighbors[6],
			PlanarGraph* _worldGraph,
			int _chunkSize,
			float _cellSize
		);

		class Generator
		{
		public:
			Generator(
				Chunk* _chunk,
				Chunk* _neighbors[6],
				PlanarGraph* _worldGraph
			) : 
				chunk(_chunk), 
				neighbors{ _neighbors[0], _neighbors[1], _neighbors[2], _neighbors[3], _neighbors[4], _neighbors[5] },
				worldGraph(_worldGraph),
				localGraph(PlanarGraph()),
				up(glm::vec2(0, chunk->initialCellSize)),
				diagRightUp(glm::vec2(chunk->initialCellSize* cos(glm::radians(30.0f)), chunk->initialCellSize* sin(glm::radians(30.0f)))),
				diagRightDown(glm::vec2(chunk->initialCellSize* cos(glm::radians(330.0f)), chunk->initialCellSize* sin(glm::radians(330.0f)))),
				centerLineStart(-glm::vec2(CHUNK_SIZE, CHUNK_SIZE) * diagRightUp)
			{
				lineIndexPrefixsum.resize(size_t(2) * chunk->chunkSize + 1);
			}

			void generateChunkTopology();

		private:
			Chunk* chunk;

			Chunk* neighbors[6];

			PlanarGraph* worldGraph;

			PlanarGraph localGraph;
			std::vector<Node*> nodesOrdered;
			std::vector<std::pair<DirectedEdge*, DirectedEdge*>> edgesOrdered;

			std::vector<size_t> lineIndexPrefixsum;

			const glm::vec2 up;
			const glm::vec2 diagRightUp;
			const glm::vec2 diagRightDown;
			const glm::vec2 centerLineStart;

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
		Cell(Chunk* _chunk, uint16_t _cellId, Node* _node)
			: chunk(_chunk), cellId(_cellId), node(_node)
		{
			completeId = (chunk->getChunkId() << 14) + cellId;

			node->setAdditionalData(this);
		}

		~Cell();

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
			return node->getPosition();
		}

		const std::vector<Cell*> getNeighbors();

	private:
		Chunk* chunk;

		uint16_t cellId;
		uint32_t completeId;

		Node* node;

		friend Chunk;
	};
}
