#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

#include "Constants.hpp"
#include "Chunk.hpp"
#include "PlanarGraph.hpp"

namespace game::world
{
	class ChunkCluster
	{
	public:
		ChunkCluster(std::vector<Chunk*>& _chunks, bool _fixBorder) : chunks(_chunks), fixBorder(_fixBorder) {};

		void relax();

		glm::vec2 getRelaxedPosition(Cell* cell)
		{
			return relaxedPositions[cell->node];
		}

	private:
		std::vector<Chunk*> chunks;

		bool fixBorder;

		std::unordered_map<Node*, glm::vec2> relaxedPositions;

		void initializeRelaxedPositions();

		void findBorder(std::unordered_set<Node*>& border);

		std::vector<Face*> calculateFaces();

		void calculateRelaxationForce(
			std::unordered_map<Node*, std::pair<glm::vec2, unsigned int>>& relaxationForces, 
			Face* face
		);

		friend std::hash<ChunkCluster>;
	};
}
