#include "ChunkCluster.hpp"

namespace game::world
{
	const float DESIRED_DIFF_TO_CENTER_LENGTH = sqrt(2.0f) * CELL_SIZE / 2.0f;

	void ChunkCluster::relax()
	{
		initializeRelaxedPositions();

		std::unordered_set<Node*> border;
		if (fixBorder)
			findBorder(border);

		std::vector<Face*> faces = calculateFaces();

		for (int i = 0; i < CLUSTER_RELAXATION_ITERATIONS; i++)
		{
			std::unordered_map<Node*, std::pair<glm::vec2, unsigned int>> relaxationForces;
			for (auto& node : relaxedPositions)
				relaxationForces.insert(std::make_pair(node.first, std::make_pair(glm::vec2(0.0f, 0.0f), 0)));

			// Calculate relaxation forces.
			for (Face* face : faces)
				calculateRelaxationForce(relaxationForces, face);

			// Update cell positions based on the relaxation forces.
			for (auto& nodeForces : relaxationForces)
			{
				if (border.find(nodeForces.first) == border.end())
				{
					std::pair<glm::vec2, unsigned int>& force = nodeForces.second;
					glm::vec2 positionDelta = CLUSTER_RELAXATION_UPDATE_WEIGHT * force.first / ((float)force.second);
					relaxedPositions[nodeForces.first] = relaxedPositions[nodeForces.first] + positionDelta;
				}
			}
		}

		for (Face* face : faces)
			delete face;
	}

	void ChunkCluster::initializeRelaxedPositions()
	{
		std::unordered_set<Cell*> cellsInCluster;

		for (Chunk* chunk : chunks)
		{
			for (Cell* cell : chunk->getCells())
				cellsInCluster.insert(cell);

			for (Cell* cell : chunk->getCellsAlongChunkBorder())
				cellsInCluster.insert(cell);
		}

		for (Cell* cell : cellsInCluster)
			relaxedPositions.insert(std::make_pair(cell->node, cell->getPosition()));
	}

	void ChunkCluster::findBorder(std::unordered_set<Node*>& border) {
		std::unordered_map<Node*, unsigned int> nodeSeenCounters;
		for (Chunk* chunk : chunks)
			for (Cell* cell : chunk->getCellsAlongChunkBorder())
				nodeSeenCounters[cell->node]++;

		for (auto& nodeSeenCounter : nodeSeenCounters)
			if (nodeSeenCounter.second == 1)
				border.insert(nodeSeenCounter.first);
	}

	std::vector<Face*> ChunkCluster::calculateFaces()
	{
		std::vector<Face*> faces;
		std::unordered_set<DirectedEdge*> traversedEdges;

		for (auto& node : relaxedPositions)
		{
			for (auto& edgeAndDestination : node.first->getEdges())
			{
				DirectedEdge* edge = edgeAndDestination.second;
				if (traversedEdges.find(edge) == traversedEdges.end())
				{
					// This edge was not yet traversed. Therefore, we have not yet calculated the face for that edge.
					Face face = edge->calculateFace();

					if (face.getNumEdges() == 4)
					{
						// All faces are quads, except for the outside of the world graph (for which we don't want to
						// perform any relaxation). Check whether all nodes of the face are within the cluster (some
						// nodes might be outside the cluster if the edge is on the border of the cluster).
						bool allNodesInCluster = true;
						for (Node* node : face.getNodes())
						{
							if (relaxedPositions.find(node) == relaxedPositions.end())
							{
								allNodesInCluster = false;
								break;
							}
						}

						if (allNodesInCluster)
						{
							// All nodes are within the cluster. Therefore this face is a quad for which we want to
							// calculate the relaxation forces in order to relax the cluster.
							faces.push_back(new Face(face));

							for (DirectedEdge* edge : face.getEdges())
								traversedEdges.insert(edge);
						}
					}
				}
			}
		}

		return faces;
	}

	void ChunkCluster::calculateRelaxationForce(
		std::unordered_map<Node*, std::pair<glm::vec2, unsigned int>>& relaxationForces,
		Face* face
	) {
		auto& nodes = face->getNodes();

		// Refer to https://twitter.com/OskSta/status/1246729301434798080/photo/1 for an image-based explanation
		// on how the relaxation forces are calculated. All variables ending with an A correspond to the red
		// point, variables ending with B to the yellow point, variables ending with C to the blue point and
		// variables ending with D to the green point.
		glm::vec2& positionA = relaxedPositions[nodes[0]];
		glm::vec2& positionB = relaxedPositions[nodes[1]];
		glm::vec2& positionC = relaxedPositions[nodes[2]];
		glm::vec2& positionD = relaxedPositions[nodes[3]];

		// Diff to center
		glm::vec2 centerPosition = (positionA + positionB + positionC + positionD) / 4.0f;

		glm::vec2 diffToCenterA = positionA - centerPosition;
		glm::vec2 diffToCenterB = positionB - centerPosition;
		glm::vec2 diffToCenterC = positionC - centerPosition;
		glm::vec2 diffToCenterD = positionD - centerPosition;

		// Rotate
		glm::vec2 diffToCenterRotatedA = diffToCenterA;
		glm::vec2 diffToCenterRotatedB = glm::vec2(-diffToCenterB.y, diffToCenterB.x);
		glm::vec2 diffToCenterRotatedC = -diffToCenterC;
		glm::vec2 diffToCenterRotatedD = glm::vec2(diffToCenterD.y, -diffToCenterD.x);

		// Average
		glm::vec2 diffToCenterAveraged =
			(diffToCenterRotatedA + diffToCenterRotatedB + diffToCenterRotatedC + diffToCenterRotatedD) / 4.0f;
		glm::vec2 diffToCenterNormalized = glm::normalize(diffToCenterAveraged) * DESIRED_DIFF_TO_CENTER_LENGTH;

		//std::cout << glm::length(diffToCenterA) << "\t" << glm::length(diffToCenterB) << "\t" << glm::length(diffToCenterC) << "\t" << glm::length(diffToCenterD) << "\t" << glm::length(diffToCenterAveraged) << std::endl;

		// Rotate back
		glm::vec2 newPosA = diffToCenterNormalized;
		glm::vec2 newPosB = glm::vec2(diffToCenterNormalized.y, -diffToCenterNormalized.x);
		glm::vec2 newPosC = -diffToCenterNormalized;
		glm::vec2 newPosD = glm::vec2(-diffToCenterNormalized.y, diffToCenterNormalized.x);

		// Move towards (not the actual movement, just the calculation of the relaxation forces)
		std::pair<glm::vec2, unsigned int>& forceA = relaxationForces[nodes[0]];
		forceA.first += newPosA - diffToCenterA;
		forceA.second++;

		std::pair<glm::vec2, unsigned int>& forceB = relaxationForces[nodes[1]];
		forceB.first += newPosB - diffToCenterB;
		forceB.second++;

		std::pair<glm::vec2, unsigned int>& forceC = relaxationForces[nodes[2]];
		forceC.first += newPosC - diffToCenterC;
		forceC.second++;

		std::pair<glm::vec2, unsigned int>& forceD = relaxationForces[nodes[3]];
		forceD.first += newPosD - diffToCenterD;
		forceD.second++;
	}
}
