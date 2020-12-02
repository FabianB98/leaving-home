#include "Chunk.hpp"

namespace game::world
{
	static const glm::vec2 UP = glm::vec2(0, CELL_SIZE);
	static const glm::vec2 DIAG_RIGHT_UP = glm::vec2(CELL_SIZE * cos(glm::radians(30.0f)), CELL_SIZE * sin(glm::radians(30.0f)));
	static const glm::vec2 DIAG_RIGHT_DOWN = glm::vec2(CELL_SIZE * cos(glm::radians(330.0f)), CELL_SIZE * sin(glm::radians(330.0f)));
	static const glm::vec2 CENTER_LINE_START = - glm::vec2(CELL_SIZE, CELL_SIZE) * DIAG_RIGHT_UP;

	void Chunk::generateMesh()
	{
		// Generate the positions of all points used as starting positions for possible cells within the current chunk.
		PlanarGraph graph = PlanarGraph();
		std::vector<Node*> nodesOrdered;
		std::vector<std::pair<DirectedEdge*, DirectedEdge*>> edgesOrdered;

		size_t sum = 0;
		size_t lineIndexPrefixsum[2 * CHUNK_SIZE + 1]{};

		for (int line = -CHUNK_SIZE; line <= CHUNK_SIZE; line++)
		{
			glm::vec2 lineStart = CENTER_LINE_START;
			if (line < 0)
				lineStart -= (float)line * UP;
			else
				lineStart += (float)line * DIAG_RIGHT_DOWN;

			int pointsInLine = 2 * CHUNK_SIZE + 1 - abs(line);
			for (int pointInLine = 0; pointInLine < pointsInLine; pointInLine++)
			{
				Node* node = new Node(lineStart + (float)pointInLine * DIAG_RIGHT_UP);
				nodesOrdered.push_back(node);
				graph.addNode(node);
			}

			lineIndexPrefixsum[line + CHUNK_SIZE] = sum;
			sum += pointsInLine;
		}

		// Create an embedding of a planar graph from the generated positions.
		// Add all edges along the first line.
		for (size_t pointInLine = 0; pointInLine < CHUNK_SIZE; pointInLine++)
		{
			edgesOrdered.push_back(graph.addEdge(nodesOrdered[pointInLine], nodesOrdered[pointInLine + 1]));
		}

		// Add all edges within the upper-left half of the hexagon.
		for (int line = 1; line <= CHUNK_SIZE; line++)
		{
			// Add the upward and right-up edge of the first point in the line.
			Node* currentNode = nodesOrdered[lineIndexPrefixsum[line]];
			edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1]]));
			edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line] + 1]));

			int pointsInLine = CHUNK_SIZE + line;
			for (int pointInLine = 1; pointInLine < pointsInLine; pointInLine++)
			{
				// Add the left-up, upward and right-up edge of the current point in the line.
				currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointInLine];
				edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine - 1]));
				edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine]));
				edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line] + pointInLine + 1]));
			}

			// Add the left-up edge of the last point in the line.
			currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointsInLine];
			edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointsInLine - 1]));
		}

		// Add all edges within the lower-right half of the hexagon.
		for (int line = CHUNK_SIZE + 1; line <= 2 * CHUNK_SIZE; line++)
		{
			int pointsInLine = 3 * CHUNK_SIZE - line;
			for (int pointInLine = 0; pointInLine < pointsInLine; pointInLine++)
			{
				// Add the left-up, upward and right-up edge of the current point in the line.
				Node* currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointInLine];
				edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine]));
				edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointInLine + 1]));
				edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line] + pointInLine + 1]));
			}
			
			// Add the left-up and upward edge of the last point in the line.
			Node* currentNode = nodesOrdered[lineIndexPrefixsum[line] + pointsInLine];
			edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointsInLine]));
			edgesOrdered.push_back(graph.addEdge(currentNode, nodesOrdered[lineIndexPrefixsum[line - 1] + pointsInLine + 1]));
		}

		// Edge removal: Iterate over all edges in a pseudorandom order and delete each edge which connects two triangles.
		std::default_random_engine random(chunkSeed);
		std::shuffle(edgesOrdered.begin(), edgesOrdered.end(), random);
		for (auto& edge : edgesOrdered)
		{
			bool forwardEdgeTriangle = edge.first
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise() == edge.first;
			bool backwardEdgeTriangle = edge.second
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise()
				->getOtherDirection()->getNextCounterclockwise() == edge.second;

			if (forwardEdgeTriangle && backwardEdgeTriangle)
			{
				delete edge.first;
			}
		}

		// TODO: Surface subdivision

		// TODO: Graph relaxation

		// Generate a mesh from the graph.
		std::vector<Face*> faces = graph.calculateFaces();

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		std::unordered_map<Node*, unsigned int> nodeIndices;
		unsigned int currentIndex = 0;
		for (auto& node : graph.getNodes())
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
			if (face->getNumNodes() == 3)
			{
				auto& nodes = face->getNodes();

				indices.push_back(nodeIndices[nodes[0]]);
				indices.push_back(nodeIndices[nodes[1]]);
				indices.push_back(nodeIndices[nodes[2]]);
			}
			else if (face->getNumNodes())
			{
				auto& nodes = face->getNodes();

				indices.push_back(nodeIndices[nodes[0]]);
				indices.push_back(nodeIndices[nodes[1]]);
				indices.push_back(nodeIndices[nodes[2]]);

				indices.push_back(nodeIndices[nodes[0]]);
				indices.push_back(nodeIndices[nodes[2]]);
				indices.push_back(nodeIndices[nodes[3]]);
			}

			delete face;
		}

		std::shared_ptr<rendering::model::Material> material = std::make_shared<rendering::model::Material>(
			glm::vec3(0.0f, 0.2f, 0.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f), 
			glm::vec3(0.0f, 0.1f, 0.0f), 
			2.0f
		);
		std::vector<std::shared_ptr<rendering::model::MeshPart>> meshParts;
		meshParts.push_back(std::make_shared<rendering::model::MeshPart>(material, indices));
		mesh = new rendering::model::Mesh(vertices, uvs, normals, meshParts);
	}
}
