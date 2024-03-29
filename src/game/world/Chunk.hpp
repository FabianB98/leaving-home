#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <random>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../rendering/bounding_geometry/AABB.hpp"
#include "../../rendering/components/CullingGeometry.hpp"
#include "../../rendering/components/MeshRenderer.hpp"
#include "../../rendering/components/Transform.hpp"
#include "../../rendering/systems/RenderingSystem.hpp"
#include "../../rendering/model/Material.hpp"
#include "../../rendering/model/Mesh.hpp"
#include "../../rendering/model/MeshPart.hpp"
#include "Constants.hpp"
#include "PlanarGraph.hpp"
#include "HeightGenerator.hpp"
#include "Inventory.hpp"

namespace game::world 
{
	class Cell;
	enum class CellType;
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

		Cell* getCellByCellId(uint16_t cellId);

		Cell* getCellByCompleteCellId(uint32_t completeCellId);

		const std::unordered_map<uint16_t, Cell*> getCells()
		{
			return cells;
		}

		const std::vector<Cell*> getCellsAlongChunkBorder()
		{
			return cellsAlongChunkBorder;
		}

		const std::unordered_set<Cell*> getCellsAndCellsAlongChunkBorder();

		rendering::model::Mesh* getTopologyMesh();

		rendering::model::Mesh* getLandscapeMesh();

		rendering::model::Mesh* getCellContentMesh()
		{
			return cellContentMesh;
		}

		HeightGenerator& getHeightGenerator()
		{
			return heightGenerator;
		}

		entt::registry& getRegistry()
		{
			return registry;
		}

		void enqueueUpdate();

	private:
		size_t chunkSeed;
		uint16_t chunkId;

		int32_t column;
		int32_t row;
		glm::vec2 centerPos;

		std::unordered_map<uint16_t, Cell*> cells;
		std::vector<Cell*> cellsAlongChunkBorder;
		std::array<glm::vec2, 6> cornerPositions;

		rendering::model::Mesh* topologyMesh;
		rendering::model::Mesh* landscapeMesh;
		rendering::model::Mesh* cellContentMesh;

		HeightGenerator& heightGenerator;

		entt::registry& registry;
		entt::entity cullingEntity{ entt::null };
		entt::entity topologyEntity{ entt::null };
		entt::entity landscapeEntity{ entt::null };
		entt::entity waterEntity{ entt::null };
		entt::entity cellContentEntity{ entt::null };

		std::shared_ptr<rendering::bounding_geometry::AABB> cullingGeometry;

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

		void generateChunkTopology(std::array<Chunk*, 6> _neighbors, PlanarGraph* _worldGraph);

		void addedToWorld();

		rendering::model::Mesh* generateWaterMesh();

		void update();

		class Generator
		{
		public:
			Generator(
				Chunk* _chunk,
				std::array<Chunk*, 6>& _neighbors,
				PlanarGraph* _worldGraph
			) : 
				chunk(_chunk), 
				neighbors(_neighbors),
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
			std::array<Chunk*, 6> neighbors;

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
				std::vector<glm::uvec2>& cellIds,
				std::unordered_map<DirectedEdge*, glm::vec2>& facePositionMap,
				unsigned int& currentIndex,
				Cell* cell,
				DirectedEdge* edge
			);

			friend class World;
		};

		friend class Cell;
		friend class World;
		friend class ChunkCluster;
	};

	struct ChunkUpdate
	{
		Chunk* chunk;
	};

	class Cell
	{
	public:
		Cell(Chunk* _chunk, uint16_t _cellId, Node* _node);

		~Cell();

		Cell* getAnyNeighborFulfillingPredicate(unsigned int range, std::function<bool (Cell*)> predicate);

		Chunk* getChunk()
		{
			return chunk;
		}

		CellContent* getContent()
		{
			return content;
		}

		void setContent(CellContent* _content)
		{
			_setContent(_content, true, true, false);
		}

		template<class T>
		void displayPlannedBuilding()
		{
			static_assert(std::is_base_of<IBuilding, T>::value, "Template parameter T must be a subclass of IBuilding!");

			if (content != nullptr && dynamic_cast<T*>(content))
			{
				content->enqueuedToAddToCell(this);
			}
			else if (content == nullptr)
			{
				std::unordered_set<T*> buildingsToConnectTo;
				for (auto cell : getNeighbors())
				{
					T* content = dynamic_cast<T*>(cell->content);
					if (cell->height == height && content)
						buildingsToConnectTo.insert(content);
				}

				if (buildingsToConnectTo.empty())
				{
					_setContent(new T(nullptr, std::unordered_set<Cell*>{}), false, false, true);
				}
				else
				{
					// The new building piece might be neighboring multiple buildings of the same type which need to be
					// connected in this case.
					T* buildingToReuse = nullptr;
					for (T* building : buildingsToConnectTo)
						if (buildingToReuse == nullptr || building->getCells().size() > buildingToReuse->getCells().size())
							buildingToReuse = building;
					Inventory& buildingToReuseInventory = chunk->getRegistry().get<Inventory>(buildingToReuse->entity);

					std::unordered_map<Cell*, BuildingHeight> cellsToPlaceBuildingOn;
					cellsToPlaceBuildingOn.insert(std::make_pair(this, BuildingHeight{ 1, 0 }));

					for (T* building : buildingsToConnectTo)
						if (building != buildingToReuse)
						{
							for (auto& cellAndHeight : building->getHeightPerCell())
								cellsToPlaceBuildingOn.insert(std::make_pair(cellAndHeight.first, cellAndHeight.second));

							// As the buildings which are to be connected to the building to reuse will be destroyed, we need
							// to move their inventory contents to the inventory of the building to reuse.
							Inventory& inventory = chunk->getRegistry().get<Inventory>(building->entity);
							buildingToReuseInventory.addItems(inventory);
						}

					for (auto& cellAndHeight : cellsToPlaceBuildingOn)
					{
						cellAndHeight.first->_setContent(buildingToReuse, false, false, false);

						for (int i = 0; i < cellAndHeight.second.plannedHeight; i++)
							buildingToReuse->enqueuedToAddToCell(cellAndHeight.first);

						for (int i = 0; i < cellAndHeight.second.actualHeight; i++)
							buildingToReuse->addedToCell(cellAndHeight.first);
					}
				}
			}
		}

		template<class T>
		bool placeBuilding()
		{
			static_assert(std::is_base_of<IBuilding, T>::value, "Template parameter T must be a subclass of IBuilding!");

			bool canBePlaced = content != nullptr && dynamic_cast<T*>(content);
			if (canBePlaced)
				content->addedToCell(this);

			return canBePlaced;
		}

		void displayPlannedRemoval();

		uint16_t getCellId()
		{
			return cellId;
		}

		uint32_t getCompleteId()
		{
			return completeId;
		}

		const std::unordered_set<Face*> getFaces()
		{
			return faces;
		}

		bool isRelaxed()
		{
			return relaxed;
		}

		glm::vec2 getUnrelaxedPosition()
		{
			return node->getPosition();
		}

		glm::vec2 getRelaxedPosition()
		{
			return relaxedPosition;
		}

		float getHeight()
		{
			return height;
		}

		glm::vec3 getUnrelaxedPositionAndHeight()
		{
			return glm::vec3(node->getPosition().x, height, node->getPosition().y);
		}

		glm::vec3 getRelaxedPositionAndHeight()
		{
			return glm::vec3(relaxedPosition.x, height, relaxedPosition.y);
		}

		CellType getCellType()
		{
			return cellType;
		}

		const std::vector<Cell*> getNeighbors();

	private:
		Chunk* chunk;
		CellContent* content;

		uint16_t cellId;
		uint32_t completeId;

		Node* node;
		std::unordered_set<Face*> faces;

		bool relaxed;
		glm::vec2 relaxedPosition;
		float height;
		CellType cellType;

		void _setContent(CellContent* _content, bool splitMultiCellContent, bool callAddedToCell, bool callEnqueuedToAddToCell);

		void setRelaxedPosition(glm::vec2 _relaxedPosition);

		void splitMultiCellContentIntoConnectedContents();

		friend Chunk;
		friend class ChunkCluster;
		friend class World;
	};

	enum class CellType
	{
		GRASS, STONE, SNOW, SAND
	};

	enum class CellHighlightStatus
	{
		NO_HIGHLIGHTING, PLANNED_FOR_CONSTRUCTION, PLANNED_FOR_DESTRUCTION
	};

	struct CellContentCellData
	{
		std::shared_ptr<rendering::model::MeshData> meshData{ nullptr };
		rendering::components::MatrixTransform transform{ glm::mat4(1.0f) };
		CellHighlightStatus highlightStatus{ CellHighlightStatus::NO_HIGHLIGHTING };
	};

	class CellContent
	{
	public:
		CellContent(bool _multiCellPlaceable, const std::string& _typeName, const std::string& _description)
			: multiCellPlaceable(_multiCellPlaceable), typeName(_typeName), description(_description) {}

		virtual ~CellContent();

		void enqueueUpdate();

		virtual void inventoryUpdated() = 0;

		const std::unordered_map<Cell*, CellContentCellData>& getCells()
		{
			return cells;
		}

		void setHighlightStatus(Cell* cell, CellHighlightStatus highlightStatus);

		bool hasMeshData();

		bool canBePlacedOnMultipleCells()
		{
			return multiCellPlaceable;
		}

		entt::registry* getRegistry()
		{
			return registry;
		}

		entt::entity& getEntity()
		{
			return entity;
		}

		const std::string& getTypeName()
		{
			return typeName;
		}

		const std::string& getDescription()
		{
			return description;
		}

		Inventory getResourcesObtainedByRemoval(Cell* cell);

		std::string getInventoryContentsString();

	protected:
		virtual CellContent* createNewCellContentOfSameType(std::unordered_set<Cell*> cellsToCopy) = 0;

		void enqueuedToAddToCell(Cell* cell);

		virtual void _enqueuedToAddToCell(Cell* cell) = 0;

		void addedToCell(Cell* cell);

		virtual void _addedToCell(Cell* cell) = 0;

		void enqueuedToRemoveFromCell(Cell* cell);

		virtual void _enqueuedToRemoveFromCell(Cell* cell) = 0;

		void removedFromCell(Cell* cell);

		virtual void _removedFromCell(Cell* cell) = 0;

		virtual void update() = 0;

		virtual const Inventory _getResourcesObtainedByRemoval(Cell* cell) = 0;

		void setMeshData(Cell* cell, std::shared_ptr<rendering::model::MeshData> meshData);

		void setTransform(Cell* cell, const rendering::components::MatrixTransform& transform);

		void setMeshDataAndTransform(
			Cell* cell,
			std::shared_ptr<rendering::model::MeshData> meshData,
			const rendering::components::MatrixTransform& transform
		);

	private:
		std::unordered_map<Cell*, CellContentCellData> cells;
		const bool multiCellPlaceable;
		const std::string& typeName;
		const std::string& description;

		entt::registry* registry{ nullptr };
		entt::entity entity{ entt::null };

		void setUpEntity(Cell* cell);

		friend Cell;
		friend class World;
	};

	struct CellContentComponent
	{
		CellContent* cellContent;
	};

	struct CellContentUpdate
	{
		CellContent* cellContent;
	};
}
