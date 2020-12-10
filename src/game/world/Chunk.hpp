#pragma once

#include <algorithm>
#include <random>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../rendering/components/MeshRenderer.hpp"
#include "../../rendering/components/Transform.hpp"
#include "../../rendering/systems/RenderingSystem.hpp"
#include "../../rendering/model/Material.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../../rendering/model/MeshPart.hpp"
#include "Constants.hpp"
#include "PlanarGraph.hpp"
#include "HeightGenerator.hpp"

namespace game::world 
{
	class Cell;
	class CellContent;

	class Chunk
	{
	public:
		Chunk(
			size_t worldSeed,
			int32_t _column,
			int32_t _row,
			HeightGenerator& _heightGenerator,
			entt::registry& _registry,
			rendering::shading::Shader* _terrainShader,
			rendering::shading::Shader* _waterShader
		) : Chunk(
			worldSeed,
			_column,
			_row,
			_heightGenerator,
			_registry,
			_terrainShader,
			_waterShader,
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

		rendering::model::Mesh* getTopologyMesh();

		rendering::model::Mesh* getLandscapeMesh();

	private:
		size_t chunkSeed;
		uint16_t chunkId;

		int32_t column;
		int32_t row;
		glm::vec2 centerPos;

		std::vector<Cell*> cells;
		std::vector<Cell*> cellsAlongChunkBorder;

		rendering::model::Mesh* topologyMesh;
		rendering::model::Mesh* landscapeMesh;

		HeightGenerator& heightGenerator;

		entt::registry& registry;

		rendering::shading::Shader* terrainShader;
		rendering::shading::Shader* waterShader;

		const int chunkSize;
		const float cellSize;

		const int numCellsAlongOneChunkEdge;
		const int numCellsAlongChunkBorder;

		const float initialCellSize;
		const float chunkBorderLength;

		const float chunkWidth;
		const float chunkHeight;

		const float chunkHorizontalDistance;
		const float chunkVerticalDistance;

		const glm::vec2 columnDirection;
		const glm::vec2 rowDirection;

		Chunk(
			size_t worldSeed,
			int32_t _column,
			int32_t _row,
			HeightGenerator& _heightGenerator,
			entt::registry& _registry,
			rendering::shading::Shader* _terrainShader,
			rendering::shading::Shader* _waterShader,
			int _chunkSize,
			float _cellSize
		);

		void generateChunkTopology(Chunk* _neighbors[6], PlanarGraph* _worldGraph);

		rendering::model::Mesh* generateWaterMesh();

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
				diagRightUp(glm::vec2(chunk->initialCellSize * cos(glm::radians(30.0f)), chunk->initialCellSize * sin(glm::radians(30.0f)))),
				diagRightDown(glm::vec2(chunk->initialCellSize * cos(glm::radians(330.0f)), chunk->initialCellSize * sin(glm::radians(330.0f)))),
				centerLineStart(-glm::vec2(chunk->chunkSize, chunk->chunkSize) * diagRightUp)
			{
				lineIndexPrefixsum.resize(size_t(2) * chunk->chunkSize + 1);
			}

			void generateChunkTopology();

			rendering::model::Mesh* generateWaterMesh();

			rendering::model::Mesh* generateTopologyGridMesh();

			rendering::model::Mesh* generateLandscapeMesh();

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

			void addCell(
				std::vector<glm::vec3>& vertices,
				std::vector<glm::vec2>& uvs,
				std::vector<glm::vec3>& normals,
				std::unordered_map<Node*, unsigned int>& nodeIndices,
				unsigned int& currentIndex,
				Cell* cell
			);

			unsigned int addCellCorner(
				std::vector<glm::vec3>& vertices,
				std::vector<glm::vec2>& uvs,
				std::vector<glm::vec3>& normals,
				std::vector<uint32_t>& cellIds,
				std::unordered_map<DirectedEdge*, glm::vec2>& facePositionMap,
				unsigned int& currentIndex,
				Cell* cell,
				DirectedEdge* edge
			);

			friend class World;
		};

		friend class Cell;
		friend class World;
	};

	class Cell
	{
	public:
		Cell(Chunk* _chunk, uint16_t _cellId, Node* _node)
			: chunk(_chunk), content(nullptr), cellId(_cellId), node(_node)
		{
			completeId = (chunk->getChunkId() << 14) + cellId;

			node->setAdditionalData(this);

			height = chunk->heightGenerator.getHeightQuantized(node->getPosition());
		}

		~Cell();

		Chunk* getChunk()
		{
			return chunk;
		}

		CellContent* getContent()
		{
			return content;
		}

		void setContent(CellContent* _content);

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

		float getHeight()
		{
			return height;
		}

		glm::vec3 getPositionAndHeight()
		{
			return glm::vec3(node->getPosition().x, height, node->getPosition().y);
		}

		const std::vector<Cell*> getNeighbors();

	private:
		Chunk* chunk;
		CellContent* content;

		uint16_t cellId;
		uint32_t completeId;

		Node* node;

		float height;

		friend Chunk;
	};

	class CellContent
	{
	public:
		CellContent(rendering::model::Mesh* _mesh) : 
			cell(nullptr),
			mesh(_mesh), 
			transform(rendering::components::MatrixTransform(glm::mat4(1.0f))) {}

		Cell* getCell()
		{
			return cell;
		}

		rendering::model::Mesh* getMesh()
		{
			return mesh;
		}

		rendering::components::MatrixTransform& getTransform()
		{
			return transform;
		}

	protected:
		Cell* cell;

		rendering::model::Mesh* mesh;
		rendering::components::MatrixTransform transform;

		virtual void addedToCell() = 0;

		friend Cell;
	};
}
