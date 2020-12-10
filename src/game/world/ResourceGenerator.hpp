#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Chunk.hpp"
#include "Constants.hpp"
#include "Resource.hpp"

namespace game::world
{
	class ResourceGenerator
	{
	public:
		ResourceGenerator(Chunk* _chunk) : chunk(_chunk) {}

		void generateResources();

	private:
		Chunk* chunk;

		void getCellsWithinDistance(Cell* startingCell, unsigned int maxMoves, std::unordered_set<Cell*>& result);
	};
}
