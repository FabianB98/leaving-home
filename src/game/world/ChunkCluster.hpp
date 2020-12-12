#pragma once

#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

#include "../../util/MathUtil.hpp"
#include "Constants.hpp"
#include "Chunk.hpp"
#include "PlanarGraph.hpp"

namespace game::world
{
	class ChunkCluster
	{
	public:
		ChunkCluster(std::vector<Chunk*>& _chunks, bool _fixBorder) : 
			chunks(_chunks), 
			fixBorder(_fixBorder), 
			relaxedPositions(std::unordered_map<Node*, glm::vec2>()) {};

		void relax();

		static void updateChunkCells(Chunk* chunk, std::array<ChunkCluster*, 6> clusters);

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

		static float calculateDeterminant(glm::vec2 a, glm::vec2 b, glm::vec2 c);

		friend std::hash<ChunkCluster>;
	};

	class ChunkClusterIdentifier
	{
	public:
		ChunkClusterIdentifier(std::vector<Chunk*>& _chunks);

		bool operator==(const ChunkClusterIdentifier& other) const;

	private:
		std::vector<Chunk*> chunks;

		friend struct std::hash<ChunkClusterIdentifier>;
	};
}

namespace std
{
	template <>
	struct hash<game::world::ChunkClusterIdentifier>
	{
		std::size_t operator()(const game::world::ChunkClusterIdentifier& identifier) const
		{
			size_t res = 17;
			for (game::world::Chunk* chunk : identifier.chunks)
				res = res * 31 + hash<game::world::Chunk*>()(chunk);

			return res;
		}
	};
}
