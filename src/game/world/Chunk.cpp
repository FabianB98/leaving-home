#include "Chunk.hpp"

#define GRASS_COLOR glm::vec3(0.16863f, 0.54902f, 0.15294f)

static rendering::model::Mesh* waterMesh;

namespace std
{
	template <>
	struct hash<pair<glm::vec2, float>>
	{
		std::size_t operator()(const pair<glm::vec2, float>& p) const
		{
			size_t res = 17;
			res = res * 31 + hash<glm::vec2>()(p.first);
			res = res * 31 + hash<float>()(p.second);
			return res;
		}
	};
}

namespace game::world
{
	Chunk::Chunk(
		size_t worldSeed,
		int32_t _column,
		int32_t _row,
		HeightGenerator& _heightGenerator,
		entt::registry& _registry,
		rendering::shading::Shader* _terrainShader,
		rendering::shading::Shader* _waterShader,
		int _chunkSize,
		float _cellSize
	) :
		column(_column),
		row(_row),
		heightGenerator(_heightGenerator),
		registry(_registry),
		terrainShader(_terrainShader),
		waterShader(_waterShader),
		chunkSize(_chunkSize),
		cellSize(_cellSize),
		numCellsAlongOneChunkEdge(2 * chunkSize),
		numCellsAlongChunkBorder(6 * numCellsAlongOneChunkEdge),
		initialCellSize(2.0f * cellSize),
		chunkBorderLength(chunkSize * initialCellSize),
		chunkWidth(sqrt(3.0f) * chunkBorderLength),
		chunkHeight(2.0f * chunkBorderLength),
		columnDirection(glm::vec2(chunkWidth, 0.0f)),
		rowDirection(chunkWidth * glm::vec2(cos(glm::radians(60.0f)), sin(glm::radians(60.0f)))),
		chunkHorizontalDistance(chunkWidth),
		chunkVerticalDistance(0.75f * chunkHeight),
		topologyMesh(nullptr),
		landscapeMesh(nullptr)
	{
		centerPos = (float)column * columnDirection + (float)row * rowDirection;

		chunkSeed = worldSeed ^ std::hash<glm::vec2>()(centerPos);

		uint16_t xId = column & 127;
		uint16_t yId = row & 127;
		chunkId = xId + (yId << 7);

		cellsAlongChunkBorder.resize(numCellsAlongChunkBorder);
		for (int i = 0; i < numCellsAlongChunkBorder; i++)
			cellsAlongChunkBorder[i] = nullptr;
	}

	void Chunk::generateChunkTopology(Chunk* _neighbors[6], PlanarGraph* _worldGraph)
	{
		Generator generator = Generator(
			this,
			_neighbors,
			_worldGraph
		);
		generator.generateChunkTopology();

		// Add an entity for the chunk's landscape mesh.
		entt::entity chunkEntity = registry.create();
		registry.emplace<rendering::components::MeshRenderer>(chunkEntity, getLandscapeMesh());
		registry.emplace<rendering::components::MatrixTransform>(
			chunkEntity,
			rendering::components::EulerComponentwiseTransform().toTransformationMatrix()
			);

		auto& shading = registry.ctx<rendering::systems::MeshShading>();
		shading.shaders.insert(std::make_pair(getLandscapeMesh(), terrainShader));

		// Add an entity for the chunk's water mesh.
		entt::entity waterEntity = registry.create();
		registry.emplace<rendering::components::MeshRenderer>(waterEntity, waterMesh);
		registry.emplace<rendering::components::MatrixTransform>(
			waterEntity,
			rendering::components::EulerComponentwiseTransform(
				glm::vec3(centerPos.x, WATER_HEIGHT, centerPos.y),
				0, 0, 0,
				glm::vec3(1)
			).toTransformationMatrix()
			);

		shading.shaders.insert(std::make_pair(waterMesh, waterShader));
		shading.priorities.insert(std::make_pair(waterShader, 1));
	}

	rendering::model::Mesh* Chunk::generateWaterMesh()
	{
		if (waterMesh != nullptr)
			delete waterMesh;

		Chunk* neighbors[6]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
		Generator generator = Generator(this, neighbors, nullptr);
		waterMesh = generator.generateWaterMesh();

		return waterMesh;
	}

	rendering::model::Mesh* Chunk::getTopologyMesh()
	{
		if (topologyMesh == nullptr)
		{
			Chunk* neighbors[6]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
			Generator generator = Generator(this, neighbors, nullptr);
			topologyMesh = generator.generateTopologyGridMesh();
		}

		return topologyMesh;
	}

	rendering::model::Mesh* Chunk::getLandscapeMesh()
	{
		if (landscapeMesh == nullptr)
		{
			Chunk* neighbors[6]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
			Generator generator = Generator(this, neighbors, nullptr);
			landscapeMesh = generator.generateLandscapeMesh();
		}

		return landscapeMesh;
	}

	Chunk::~Chunk()
	{
		if (topologyMesh != nullptr)
			delete topologyMesh;

		if (landscapeMesh != nullptr)
			delete landscapeMesh;

		for (auto& cell : cells)
			delete cell;
	}

	void Chunk::Generator::generateChunkTopology()
	{
		generateInitialPositions();
		generateInitialEdges();

		removeEdges();
		subdivideSurfaces();

		setChunkTopologyData();

		if (GENERATE_TOPOLOGY_MESH_BY_DEFAULT)
			chunk->topologyMesh = generateTopologyGridMesh();

		if (GENERATE_LANDSCAPE_MESH_BY_DEFAULT)
			chunk->landscapeMesh = generateLandscapeMesh();
	}

	void Chunk::Generator::generateInitialPositions()
	{
		// Generate the positions of all points used as starting positions for possible cells within the current chunk.
		// Positions are generated in lines. Each line starts at the bottom left most point of the line and is generated
		// by moving up diagonally to the right.
		size_t sum = 0;
		glm::vec2 leftMostPosition = chunk->centerPos + centerLineStart;
		for (int line = -chunk->chunkSize; line <= chunk->chunkSize; line++)
		{
			glm::vec2 lineStart = leftMostPosition;
			if (line < 0)
				lineStart -= (float)line * up;
			else
				lineStart += (float)line * diagRightDown;

			int pointsInLine = 2 * chunk->chunkSize + 1 - abs(line);
			for (int pointInLine = 0; pointInLine < pointsInLine; pointInLine++)
			{
				Node* node = new Node(lineStart + (float)pointInLine * diagRightUp);
				nodesOrdered.push_back(node);
				localGraph.addNode(node);
			}

			lineIndexPrefixsum[line + chunk->chunkSize] = sum;
			sum += pointsInLine;
		}
	}

	void Chunk::Generator::generateInitialEdges()
	{
		// Create an embedding of a planar graph from the generated positions.
		// Add all edges along the first line.
		for (size_t pointInLine = 0; pointInLine < chunk->chunkSize; pointInLine++)
		{
			edgesOrdered.push_back(localGraph.addEdge(nodesOrdered[pointInLine], nodesOrdered[pointInLine + 1]));
		}

		// Add all edges within the upper-left half of the hexagon.
		for (int line = 1; line <= chunk->chunkSize; line++)
		{
			// Add the upward and right-up edge of the first point in the line.
			Node* currentNode = nodesOrdered[lineIndexPrefixsum[line]];
			edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1]]));
			edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line] + 1]));

			int pointsInLine = chunk->chunkSize + line;
			for (int pointInLine = 1; pointInLine < pointsInLine; pointInLine++)
			{
				// Add the left-up, upward and right-up edge of the current point in the line.
				currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointInLine];
				edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine - 1]));
				edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine]));
				edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line] + pointInLine + 1]));
			}

			// Add the left-up edge of the last point in the line.
			currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointsInLine];
			edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointsInLine - 1]));
		}

		// Add all edges within the lower-right half of the hexagon.
		for (int line = chunk->chunkSize + 1; line <= 2 * chunk->chunkSize; line++)
		{
			int pointsInLine = 3 * chunk->chunkSize - line;
			for (int pointInLine = 0; pointInLine < pointsInLine; pointInLine++)
			{
				// Add the left-up, upward and right-up edge of the current point in the line.
				Node* currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointInLine];
				edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine]));
				edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine + 1]));
				edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line] + pointInLine + 1]));
			}

			// Add the left-up and upward edge of the last point in the line.
			Node* currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointsInLine];
			edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointsInLine]));
			edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointsInLine + 1]));
		}
	}

	void Chunk::Generator::removeEdges()
	{
		// Edge removal: Iterate over all edges in a pseudorandom order and delete each edge which connects two triangles.
		std::default_random_engine random(chunk->chunkSeed);
		std::shuffle(edgesOrdered.begin(), edgesOrdered.end(), random);
		auto& edge = edgesOrdered.begin();
		while (edge != edgesOrdered.end())
		{
			bool forwardEdgeTriangle = edge->first
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise() == edge->first;
			bool backwardEdgeTriangle = edge->second
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise() == edge->second;

			if (forwardEdgeTriangle && backwardEdgeTriangle)
			{
				delete edge->first;
				edge = edgesOrdered.erase(edge);
			}
			else
			{
				edge++;
			}
		}
	}

	void Chunk::Generator::subdivideSurfaces()
	{
		// Surface subdivision: Divide each triangle face into 3 quads and each quad face into 4 quads by inserting a
		// new node at the center of the face and connecting it to all nodes of that face as well as the center points
		// of each edge of that face.
		std::unordered_set<glm::vec2> connectToPositions;
		for (auto& edge : edgesOrdered)
		{
			Node* fromNode = edge.first->getFrom();
			Node* toNode = edge.first->getTo();
			Node* centerNode = new Node(0.5f * (fromNode->getPosition() + toNode->getPosition()));

			delete edge.first;
			localGraph.addEdge(centerNode, fromNode);
			localGraph.addEdge(centerNode, toNode);

			connectToPositions.insert(centerNode->getPosition());
		}

		std::vector<Face*> faces = localGraph.calculateFaces();
		for (auto& face : faces)
		{
			size_t nodesInFace = face->getNumNodes();
			if (nodesInFace == 6 || nodesInFace == 8)
			{
				glm::vec2 centerNodePosition = glm::vec2(0.0f, 0.0f);
				for (auto& node : face->getNodes())
					centerNodePosition += node->getPosition();
				centerNodePosition /= (float)nodesInFace;

				Node* centerNode = new Node(centerNodePosition);

				for (auto& node : face->getNodes())
				{
					if (connectToPositions.find(node->getPosition()) != connectToPositions.end())
					{
						localGraph.addEdge(centerNode, node);
					}
				}
			}

			delete face;
		}
	}

	void Chunk::Generator::setChunkTopologyData()
	{
		// Create a map mapping from the local nodes along the chunk border to their corresponding index within the
		// cellsAlongChunkBorder array. 
		std::unordered_map<Node*, int> borderIndexMap;
		std::unordered_map<int, Node*> indexBorderMap;
		std::unordered_set<DirectedEdge*> traversedEdges;

		Node* topNode = nodesOrdered[lineIndexPrefixsum[1] - 1];
		Node* nodeIndexTwo = nodesOrdered[lineIndexPrefixsum[1] - 2];

		DirectedEdge* edge = topNode->getEdges().begin()->second;
		while (edge->getOtherDirection()->getNextCounterclockwise()->getTo() != nodeIndexTwo)
			edge = edge->getNextCounterclockwise();

		for (int i = 0; i < chunk->numCellsAlongChunkBorder; i++)
		{
			int index = (i + 3 * chunk->numCellsAlongOneChunkEdge) % chunk->numCellsAlongChunkBorder;
			borderIndexMap.insert(std::make_pair(edge->getFrom(), index));
			indexBorderMap.insert(std::make_pair(index, edge->getFrom()));
			edge = edge->getOtherDirection()->getNextCounterclockwise();
		}

		// Create global copies of all local nodes which don't exist in the world graph yet and store a map mapping from
		// local nodes to their corresponding global nodes.
		std::unordered_map<Node*, Node*> nodeLocalToGlobalMap;
		for (int chunkEdge = 0; chunkEdge < 6; chunkEdge++)
		{
			if (neighbors[chunkEdge] != nullptr)
			{
				// The current chunk has an already existing neighbor chunk on the current edge. Therefore, we need to
				// share all nodes which are on the current edge between this chunk and the neighboring chunk.
				int cellsAlongBorderStart = chunkEdge * chunk->numCellsAlongOneChunkEdge;
				Node* previousLocalNode = nullptr;
				for (int i = 0; i <= chunk->numCellsAlongOneChunkEdge; i++)
				{
					// Set the cellsAlongChunkBorder array for this chunk to reflect that we're sharing this cell with
					// the neighboring chunk.
					int neighborIndexWithOverflow = cellsAlongBorderStart + 4 * chunk->numCellsAlongOneChunkEdge - i;
					int neighborIndex = neighborIndexWithOverflow % chunk->numCellsAlongChunkBorder;
					Cell* cell = neighbors[chunkEdge]->cellsAlongChunkBorder[neighborIndex];
					int ownIndex = (cellsAlongBorderStart + i) % chunk->numCellsAlongChunkBorder;
					chunk->cellsAlongChunkBorder[ownIndex] = cell;

					// Store that this local node will be mapped to the shared global node of the shared cell.
					Node* localNode = indexBorderMap[ownIndex];
					nodeLocalToGlobalMap.insert(std::make_pair(localNode, cell->node));

					// As we're already sharing all nodes on this chunk edge with the neighboring chunk, we don't need
					// to add the graph edges along this chunk edge to the global graph again. To ensure that graph
					// edges along this chunk edge won't be added to the global graph again, we mark them as traversed.
					if (previousLocalNode != nullptr)
					{
						DirectedEdge* edge = previousLocalNode->getEdge(localNode);
						traversedEdges.insert(edge);
						traversedEdges.insert(edge->getOtherDirection());
					}
					previousLocalNode = localNode;
				}
			}
		}

		uint16_t cellId = 0;
		for (auto& iterator : localGraph.getNodes())
		{
			Node* const & localNode = iterator.second;
			if (nodeLocalToGlobalMap.find(localNode) == nodeLocalToGlobalMap.end())
			{
				// The current local node is not part of some border to an already existing chunk. We must create a new
				// global node (i.e. a node in the world graph) and a cell for it.
				Node* globalNode = new Node(localNode->getPosition());
				worldGraph->addNode(globalNode);
				nodeLocalToGlobalMap.insert(std::make_pair(localNode, globalNode));

				Cell* cell = new Cell(chunk, cellId, globalNode);
				chunk->cells.push_back(cell);
				cellId++;

				auto& index = borderIndexMap.find(localNode);
				if (index != borderIndexMap.end())
					chunk->cellsAlongChunkBorder[index->second] = cell;
			}
		}

		// Copy the edges from the local graph to the world graph.
		for (auto& localNode : localGraph.getNodes())
		{
			Node* worldNode = nodeLocalToGlobalMap[localNode.second];
			for (auto& outgoingEdge : localNode.second->getEdges())
			{
				if (traversedEdges.find(outgoingEdge.second) == traversedEdges.end())
				{
					worldNode->addEdgeTo(nodeLocalToGlobalMap[outgoingEdge.second->getTo()]);

					traversedEdges.insert(outgoingEdge.second);
					traversedEdges.insert(outgoingEdge.second->getOtherDirection());
				}
			}
		}
	}

	void Chunk::Generator::addCell(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		std::unordered_map<Node*, unsigned int>& nodeIndices,
		unsigned int& currentIndex,
		Cell* cell
	) {
		if (nodeIndices.find(cell->node) == nodeIndices.end())
		{
			vertices.push_back(cell->getPositionAndHeight());
			uvs.push_back(glm::vec2(0, 0));
			normals.push_back(glm::vec3(0, 1, 0));

			nodeIndices.insert(std::make_pair(cell->node, currentIndex));
			currentIndex++;
		}
	}

	rendering::model::Mesh* Chunk::Generator::generateTopologyGridMesh()
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		std::unordered_map<Node*, unsigned int> nodeIndices;
		unsigned int currentIndex = 0;

		for (auto& cell : chunk->cells)
			addCell(vertices, uvs, normals, nodeIndices, currentIndex, cell);
		for (auto& cell : chunk->cellsAlongChunkBorder)
			addCell(vertices, uvs, normals, nodeIndices, currentIndex, cell);

		std::unordered_set<DirectedEdge*> traversedEdges;
		for (Cell* cell : chunk->cells)
		{
			Node* node = cell->node;
			for (auto& edgeAndDestination : node->getEdges())
			{
				DirectedEdge* outgoingEdge = edgeAndDestination.second;
				if (traversedEdges.find(outgoingEdge) == traversedEdges.end())
				{
					Face face = outgoingEdge->calculateFace();

					// All faces of the planar graph are quads (except for the outside of the chunk, which we don't
					// want to render).
					if (face.getNumEdges() == 4)
					{
						for (DirectedEdge* edge : face.getEdges())
						{
							indices.push_back(nodeIndices[edge->getFrom()]);
							indices.push_back(nodeIndices[edge->getTo()]);

							traversedEdges.insert(edge);
						}
					}
					else
					{
						for (DirectedEdge* edge : face.getEdges())
						{
							traversedEdges.insert(edge);
						}
					}
				}
			}
		}

		std::shared_ptr<rendering::model::Material> material = std::make_shared<rendering::model::Material>(
			glm::vec3(0.2f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.1f, 0.0f, 0.0f),
			2.0f
			);
		std::vector<std::shared_ptr<rendering::model::MeshPart>> meshParts;
		meshParts.push_back(std::make_shared<rendering::model::MeshPart>(material, indices, GL_LINES));
		return new rendering::model::Mesh(vertices, uvs, normals, meshParts);
	}

	unsigned int Chunk::Generator::addCellCorner(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		std::vector<uint32_t>& cellIds,
		std::unordered_map<DirectedEdge*, glm::vec2>& facePositionMap,
		unsigned int& currentIndex,
		Cell* cell,
		DirectedEdge* edge
	) {
		glm::vec2& position = facePositionMap[edge];

		vertices.push_back(glm::vec3(position.x, cell->height, position.y));
		uvs.push_back(glm::vec2(0.0f, 0.0f));
		normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		cellIds.push_back(cell->completeId);

		return currentIndex++;
	}

	rendering::model::Mesh* Chunk::Generator::generateLandscapeMesh()
	{
		// Determine the center positions of all planar graph faces within the chunk.
		std::unordered_set<Cell*> cellsToCalculateFacesFor;
		for (Cell* cell : chunk->cells)
			cellsToCalculateFacesFor.insert(cell);
		for (Cell* cell : chunk->cellsAlongChunkBorder)
			cellsToCalculateFacesFor.insert(cell);

		std::unordered_map<DirectedEdge*, glm::vec2> facePositionMap;
		std::unordered_set<DirectedEdge*> traversedEdges;
		for (Cell* cell : cellsToCalculateFacesFor)
		{
			Node* node = cell->node;
			for (auto& edgeAndDestination : node->getEdges())
			{
				DirectedEdge* outgoingEdge = edgeAndDestination.second;
				if (traversedEdges.find(outgoingEdge) == traversedEdges.end())
				{
					Face face = outgoingEdge->calculateFace();

					// All faces of the planar graph are quads (except for the outside of the chunk, which we don't
					// want to render).
					if (face.getNumEdges() == 4)
					{
						glm::vec2 faceCenterPos = glm::vec2(0.0f, 0.0f);
						for (Node* node : face.getNodes())
							faceCenterPos += node->getPosition();
						faceCenterPos /= 4.0f;

						for (DirectedEdge* edge : face.getEdges())
						{
							facePositionMap.insert(std::make_pair(edge, faceCenterPos));

							traversedEdges.insert(edge);
						}
					}
					else
					{
						for (DirectedEdge* edge : face.getEdges())
						{
							traversedEdges.insert(edge);
						}
					}
				}
			}
		}

		// Determine all cells which we need to add to the mesh.
		std::unordered_set<Cell*> cellsToTraverse;
		for (Cell* cell : chunk->cells)
			cellsToTraverse.insert(cell);

		for (Cell* cell : chunk->cellsAlongChunkBorder)
		{
			if (cell->chunk == chunk)
				cellsToTraverse.erase(cell);
			else
				cellsToTraverse.insert(cell);
		}

		// Create the mesh.
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<uint32_t> cellIds;

		std::vector<unsigned int> indices;
		unsigned int index = 0;

		traversedEdges.clear();

		for (Cell* cell : cellsToTraverse)
		{
			bool allFacesDefined = true;
			for (auto& edge : cell->node->getEdges())
			{
				if (facePositionMap.find(edge.second) == facePositionMap.end())
					allFacesDefined = false;
			}
			
			if (allFacesDefined)
			{
				// Add triangles for the upwards facing face of the cell.
				DirectedEdge* startingEdge = cell->node->getEdges().begin()->second;
				int startIndex = addCellCorner(vertices, uvs, normals, cellIds, facePositionMap, index, cell, startingEdge);

				DirectedEdge* currentEdge = startingEdge->getNextClockwise();
				int lastIndex = addCellCorner(vertices, uvs, normals, cellIds, facePositionMap, index, cell, currentEdge);

				currentEdge = currentEdge->getNextClockwise();
				while (currentEdge != startingEdge)
				{
					int currentIndex = addCellCorner(vertices, uvs, normals, cellIds, facePositionMap, index, cell, currentEdge);

					indices.push_back(startIndex);
					indices.push_back(lastIndex);
					indices.push_back(currentIndex);

					lastIndex = currentIndex;
					currentEdge = currentEdge->getNextClockwise();
				}

				// Add triangles for the sideways facing faces of the cell if needed.
				DirectedEdge* lastEdge = startingEdge;
				currentEdge = lastEdge->getNextClockwise();
				do
				{
					Cell* otherCell = (Cell*)currentEdge->getTo()->getAdditionalData();

					float ownHeight = cell->getHeight();
					float otherHeight = otherCell->getHeight();
					if (ownHeight != otherHeight && traversedEdges.find(currentEdge) == traversedEdges.end())
					{
						traversedEdges.insert(currentEdge);
						traversedEdges.insert(currentEdge->getOtherDirection());

						glm::vec2 cornerPosA = facePositionMap[currentEdge];
						glm::vec2 cornerPosB = facePositionMap[lastEdge];

						glm::vec3 cornerPosDiff = glm::vec3(cornerPosA.x, 0, cornerPosA.y) - glm::vec3(cornerPosB.x, 0, cornerPosB.y);
						glm::vec3 normal = glm::normalize(glm::cross(cornerPosDiff, glm::vec3(0, 1, 0)));
						uint32_t cellId;
						if (ownHeight < otherHeight)
						{
							normal = -normal;
							cellId = otherCell->completeId;
						}
						else
						{
							cellId = cell->completeId;
						}

						vertices.push_back(glm::vec3(cornerPosA.x, ownHeight, cornerPosA.y));
						uvs.push_back(glm::vec2(0.0f, 0.0f));
						normals.push_back(normal);
						cellIds.push_back(cellId);

						vertices.push_back(glm::vec3(cornerPosB.x, ownHeight, cornerPosB.y));
						uvs.push_back(glm::vec2(0.0f, 0.0f));
						normals.push_back(normal);
						cellIds.push_back(cellId);

						vertices.push_back(glm::vec3(cornerPosB.x, otherHeight, cornerPosB.y));
						uvs.push_back(glm::vec2(0.0f, 0.0f));
						normals.push_back(normal);
						cellIds.push_back(cellId);

						vertices.push_back(glm::vec3(cornerPosA.x, otherHeight, cornerPosA.y));
						uvs.push_back(glm::vec2(0.0f, 0.0f));
						normals.push_back(normal);
						cellIds.push_back(cellId);

						indices.push_back(index);
						indices.push_back(index + 1);
						indices.push_back(index + 2);

						indices.push_back(index);
						indices.push_back(index + 2);
						indices.push_back(index + 3);

						index += 4;
					}

					lastEdge = currentEdge;
					currentEdge = currentEdge->getNextClockwise();
				} while (lastEdge != startingEdge);
			}
		}

		std::shared_ptr<rendering::model::Material> material = std::make_shared<rendering::model::Material>(
			0.2f * GRASS_COLOR,
			0.5f * GRASS_COLOR,
			0.3f * GRASS_COLOR,
			2.0f
			);
		std::vector<std::shared_ptr<rendering::model::MeshPart>> meshParts;
		meshParts.push_back(std::make_shared<rendering::model::MeshPart>(material, indices, GL_TRIANGLES));
		rendering::model::Mesh* mesh = new rendering::model::Mesh(vertices, uvs, normals, meshParts);
		mesh->addAdditionalVertexAttributeI<uint32_t>(CELL_ID_ATTRIBUTE_LOCATION, cellIds, 1, GL_UNSIGNED_INT);
		return mesh;
	}

	rendering::model::Mesh* Chunk::Generator::generateWaterMesh()
	{
		generateInitialPositions();
		generateInitialEdges();

		std::vector<Face*> faces = localGraph.calculateFaces();

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		std::unordered_map<Node*, unsigned int> nodeIndices;
		unsigned int currentIndex = 0;
		for (auto& node : localGraph.getNodes())
		{
			glm::vec2& position = node.second->getPosition();
			vertices.push_back(glm::vec3(position.x, 0, position.y));
			uvs.push_back(glm::vec2(0, 0));
			normals.push_back(glm::vec3(0, 1, 0));

			nodeIndices.insert(std::make_pair(node.second, currentIndex));
			currentIndex++;
		}

		std::vector<unsigned int> indices;

		for (auto& face : faces)
		{
			size_t nodesInFace = face->getNumNodes();
			if (nodesInFace == 3)
			{
				// All faces of the planar graph are triangles (except for the outside of the chunk, which we don't want to render).
				auto& nodes = face->getNodes();

				indices.push_back(nodeIndices[nodes[0]]);
				indices.push_back(nodeIndices[nodes[1]]);
				indices.push_back(nodeIndices[nodes[2]]);
			}

			delete face;
		}

		std::shared_ptr<rendering::model::Material> material = std::make_shared<rendering::model::Material>(
			glm::vec3(0.0f, 0.0f, 0.1f),
			glm::vec3(0.0f, 0.0f, 0.3f),
			glm::vec3(0.2f, 0.2f, 0.4f),
			24.0f
			);
		std::vector<std::shared_ptr<rendering::model::MeshPart>> meshParts;
		meshParts.push_back(std::make_shared<rendering::model::MeshPart>(material, indices, GL_TRIANGLES));
		return new rendering::model::Mesh(vertices, uvs, normals, meshParts);
	}

	Cell::Cell(Chunk* _chunk, uint16_t _cellId, Node* _node)
		: chunk(_chunk), content(nullptr), cellId(_cellId), node(_node)
	{
		completeId = (chunk->getChunkId() << 14) + cellId;
		entity = chunk->registry.create();

		node->setAdditionalData(this);

		height = chunk->heightGenerator.getHeightQuantized(node->getPosition());
	}

	Cell::~Cell()
	{
		if (content != nullptr)
			delete content;

		delete node;
	}

	void Cell::setContent(CellContent* _content)
	{
		if (_content != nullptr && _content->cell != nullptr)
			return;

		if (content != nullptr)
			delete content;

		content = _content;
		if (content != nullptr)
		{
			content->cell = this;
			content->addedToCell();

			entt::entity entity = chunk->registry.create();
			chunk->registry.emplace<rendering::components::MeshRenderer>(entity, content->getMesh());
			chunk->registry.emplace<rendering::components::MatrixTransform>(entity, content->getTransform());
		}
	}

	const std::vector<Cell*> Cell::getNeighbors()
	{
		std::vector<Cell*> neighbors;

		for (auto& edge : node->getEdges())
			neighbors.push_back((Cell*)(edge.first->getAdditionalData()));

		return neighbors;
	}
}
