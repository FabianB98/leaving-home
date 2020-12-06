#include "Chunk.hpp"

namespace game::world
{
	static const int INITIAL_CELL_SIZE = 2 * CELL_SIZE;
	static const int CHUNK_BORDER_LENGTH = CHUNK_SIZE * INITIAL_CELL_SIZE;
	static const int TOTAL_BORDER_LENGTH = 6 * CHUNK_BORDER_LENGTH;

	static const float CHUNK_WIDTH = sqrt(3.0f) * CHUNK_BORDER_LENGTH;
	static const float CHUNK_HEIGHT = 2 * CHUNK_BORDER_LENGTH;

	static const float CHUNK_HORIZONTAL_DISTANCE = CHUNK_WIDTH;
	static const float CHUNK_VERTICAL_DISTANCE = 0.75 * CHUNK_HEIGHT;

	static const glm::vec2 UP = glm::vec2(0, INITIAL_CELL_SIZE);
	static const glm::vec2 DIAG_RIGHT_UP = glm::vec2(INITIAL_CELL_SIZE * cos(glm::radians(30.0f)), INITIAL_CELL_SIZE * sin(glm::radians(30.0f)));
	static const glm::vec2 DIAG_RIGHT_DOWN = glm::vec2(INITIAL_CELL_SIZE * cos(glm::radians(330.0f)), INITIAL_CELL_SIZE * sin(glm::radians(330.0f)));
	static const glm::vec2 CENTER_LINE_START = - glm::vec2(CHUNK_SIZE, CHUNK_SIZE) * DIAG_RIGHT_UP;

	Chunk::Chunk(
		size_t worldSeed,
		int32_t _column,
		int32_t _row,
		Chunk* _neighbors[6],
		PlanarGraph* _worldGraph
	) : column(_column), row(_row), mesh(nullptr)
	{
		float xPos = (column + (row % 2) * 0.5f) * CHUNK_HORIZONTAL_DISTANCE;
		float yPos = row * CHUNK_VERTICAL_DISTANCE;
		centerPos = glm::vec2(xPos, yPos);

		chunkSeed = worldSeed ^ std::hash<glm::vec2>()(centerPos);

		uint16_t xId = column & 127;
		uint16_t yId = row & 127;
		chunkId = xId + (yId << 7);

		for (int i = 0; i < 6 * CHUNK_BORDER_LENGTH; i++)
			cellsAlongChunkBorder[i] = nullptr;

		Generator generator = Generator(
			this,
			_neighbors,
			_worldGraph
		);
		generator.generateChunkTopology();
	}

	Chunk::~Chunk()
	{
		delete mesh;

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
	}

	void Chunk::Generator::generateInitialPositions()
	{
		// Generate the positions of all points used as starting positions for possible cells within the current chunk.
		// Positions are generated in lines. Each line starts at the bottom left most point of the line and is generated
		// by moving up diagonally to the right.
		size_t sum = 0;
		glm::vec2 centerLineStart = chunk->centerPos + CENTER_LINE_START;
		for (int line = -CHUNK_SIZE; line <= CHUNK_SIZE; line++)
		{
			glm::vec2 lineStart = centerLineStart;
			if (line < 0)
				lineStart -= (float)line * UP;
			else
				lineStart += (float)line * DIAG_RIGHT_DOWN;

			int pointsInLine = 2 * CHUNK_SIZE + 1 - abs(line);
			for (int pointInLine = 0; pointInLine < pointsInLine; pointInLine++)
			{
				Node* node = new Node(lineStart + (float)pointInLine * DIAG_RIGHT_UP);
				nodesOrdered.push_back(node);
				localGraph.addNode(node);
			}

			lineIndexPrefixsum[line + CHUNK_SIZE] = sum;
			sum += pointsInLine;
		}
	}

	void Chunk::Generator::generateInitialEdges()
	{
		// Create an embedding of a planar graph from the generated positions.
		// Add all edges along the first line.
		for (size_t pointInLine = 0; pointInLine < CHUNK_SIZE; pointInLine++)
		{
			edgesOrdered.push_back(localGraph.addEdge(nodesOrdered[pointInLine], nodesOrdered[pointInLine + 1]));
		}

		// Add all edges within the upper-left half of the hexagon.
		for (int line = 1; line <= CHUNK_SIZE; line++)
		{
			// Add the upward and right-up edge of the first point in the line.
			Node* currentNode = nodesOrdered[lineIndexPrefixsum[line]];
			edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1]]));
			edgesOrdered.push_back(localGraph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line] + 1]));

			int pointsInLine = CHUNK_SIZE + line;
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
		for (int line = CHUNK_SIZE + 1; line <= 2 * CHUNK_SIZE; line++)
		{
			int pointsInLine = 3 * CHUNK_SIZE - line;
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
		std::unordered_set<DirectedEdge*> traversedEdges;

		Node* topNode = nodesOrdered[lineIndexPrefixsum[1] - 1];
		Node* nodeIndexTwo = nodesOrdered[lineIndexPrefixsum[1] - 2];

		DirectedEdge* edge = topNode->getEdges().begin()->second;
		while (edge->getOtherDirection()->getNextCounterclockwise()->getTo() != nodeIndexTwo)
			edge = edge->getNextCounterclockwise();

		for (int i = 0; i < TOTAL_BORDER_LENGTH; i++)
		{
			borderIndexMap.insert(std::make_pair(edge->getFrom(), (i + 3 * CHUNK_BORDER_LENGTH) % TOTAL_BORDER_LENGTH));
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
				int cellsAlongBorderStart = chunkEdge * CHUNK_BORDER_LENGTH;
				Node* previousLocalNode = nullptr;
				for (int i = 0; i <= CHUNK_BORDER_LENGTH; i++)
				{
					// Set the cellsAlongChunkBorder array for this chunk to reflect that we're sharing this cell with
					// the neighboring chunk.
					int neighborIndex = (cellsAlongBorderStart + 4 * CHUNK_BORDER_LENGTH - i) % TOTAL_BORDER_LENGTH;
					Cell* cell = neighbors[chunkEdge]->cellsAlongChunkBorder[neighborIndex];
					chunk->cellsAlongChunkBorder[(cellsAlongBorderStart + i) % TOTAL_BORDER_LENGTH] = cell;

					// Store that this local node will be mapped to the shared global node of the shared cell.
					Node* localNode = localGraph.getNodeAt(cell->node->getPosition());
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

		// TODO: The mesh shouldn't be directly generated here. Therefore all the code starting from here and ending at
		// the end of this function should be moved to somewhere else.

		// Generate a mesh from the graph.
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
			if (nodesInFace == 4)
			{
				// All faces of the planar graph are quads (except for the outside of the chunk, which we don't want to render).
				auto& nodes = face->getNodes();

				indices.push_back(nodeIndices[nodes[0]]);
				indices.push_back(nodeIndices[nodes[1]]);

				indices.push_back(nodeIndices[nodes[1]]);
				indices.push_back(nodeIndices[nodes[2]]);

				indices.push_back(nodeIndices[nodes[2]]);
				indices.push_back(nodeIndices[nodes[3]]);

				indices.push_back(nodeIndices[nodes[3]]);
				indices.push_back(nodeIndices[nodes[0]]);
			}

			delete face;
		}

		// START OF TEMPORARY TEST CODE
		glm::vec2 up = chunk->centerPos + (float)CHUNK_SIZE * UP;
		glm::vec2 rightUp = chunk->centerPos + (float)CHUNK_SIZE * DIAG_RIGHT_UP;
		glm::vec2 rightDown = chunk->centerPos + (float)CHUNK_SIZE * DIAG_RIGHT_DOWN;
		glm::vec2 down = chunk->centerPos - (float)CHUNK_SIZE * UP;
		glm::vec2 leftDown = chunk->centerPos - (float)CHUNK_SIZE * DIAG_RIGHT_UP;
		glm::vec2 leftUp = chunk->centerPos - (float)CHUNK_SIZE * DIAG_RIGHT_DOWN;

		vertices.push_back(glm::vec3(up.x, 0, up.y));
		uvs.push_back(glm::vec2(0, 0));
		normals.push_back(glm::vec3(0, 1, 0));

		vertices.push_back(glm::vec3(rightUp.x, 1, rightUp.y));
		uvs.push_back(glm::vec2(0, 0));
		normals.push_back(glm::vec3(0, 1, 0));

		vertices.push_back(glm::vec3(rightDown.x, 2, rightDown.y));
		uvs.push_back(glm::vec2(0, 0));
		normals.push_back(glm::vec3(0, 1, 0));

		vertices.push_back(glm::vec3(down.x, 3, down.y));
		uvs.push_back(glm::vec2(0, 0));
		normals.push_back(glm::vec3(0, 1, 0));

		vertices.push_back(glm::vec3(leftDown.x, 4, leftDown.y));
		uvs.push_back(glm::vec2(0, 0));
		normals.push_back(glm::vec3(0, 1, 0));

		vertices.push_back(glm::vec3(leftUp.x, 5, leftUp.y));
		uvs.push_back(glm::vec2(0, 0));
		normals.push_back(glm::vec3(0, 1, 0));

		std::vector<unsigned int> debugIndices;

		debugIndices.push_back(nodeIndices.size() + 0);
		debugIndices.push_back(nodeIndices.size() + 1);

		debugIndices.push_back(nodeIndices.size() + 1);
		debugIndices.push_back(nodeIndices.size() + 2);

		debugIndices.push_back(nodeIndices.size() + 2);
		debugIndices.push_back(nodeIndices.size() + 3);

		debugIndices.push_back(nodeIndices.size() + 3);
		debugIndices.push_back(nodeIndices.size() + 4);

		debugIndices.push_back(nodeIndices.size() + 4);
		debugIndices.push_back(nodeIndices.size() + 5);

		std::shared_ptr<rendering::model::Material> debugMaterial = std::make_shared<rendering::model::Material>(
			glm::vec3(0.2f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.1f, 0.0f, 0.0f),
			2.0f
			);
		// END OF TEMPORARY TEST CODE

		std::shared_ptr<rendering::model::Material> material = std::make_shared<rendering::model::Material>(
			glm::vec3(0.0f, 0.2f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.1f, 0.0f),
			2.0f
			);
		std::vector<std::shared_ptr<rendering::model::MeshPart>> meshParts;
		meshParts.push_back(std::make_shared<rendering::model::MeshPart>(material, indices, GL_LINES));
		meshParts.push_back(std::make_shared<rendering::model::MeshPart>(debugMaterial, debugIndices, GL_LINES));
		chunk->mesh = new rendering::model::Mesh(vertices, uvs, normals, meshParts);
	}

	Cell::~Cell()
	{
		delete node;
	}

	const std::vector<Cell*> Cell::getNeighbors()
	{
		std::vector<Cell*> neighbors;

		for (auto& edge : node->getEdges())
			neighbors.push_back((Cell*)(edge.first->getAdditionalData()));

		return neighbors;
	}
}
