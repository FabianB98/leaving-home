#include "ResourceGenerator.hpp"

namespace game::world
{
	void ResourceGenerator::generateResources()
	{
		// Determine the noise for all cells of the chunk and all neighboring cells that can be reached within
		// TREE_DENSITY moves.
		std::unordered_set<Cell*> cellsToCalculateNoiseFor;
		for (Cell* cell : chunk->getCells())
			getCellsWithinDistance(cell, TREE_DENSITY, cellsToCalculateNoiseFor);

		std::unordered_map<Cell*, float> noiseMap;
		for (Cell* cell : cellsToCalculateNoiseFor)
			noiseMap.insert(std::make_pair(cell, chunk->getHeightGenerator().getBlueNoise(cell->getPosition())));

		// Generate resources for all cells of the chunk that are not located on the chunk's border.
		std::unordered_set<Cell*> cellsToGenerateResourcesFor;
		for (Cell* cell : chunk->getCells())
			cellsToGenerateResourcesFor.insert(cell);
		for (Cell* cell : chunk->getCellsAlongChunkBorder())
			cellsToGenerateResourcesFor.erase(cell);

		for (Cell* cell : cellsToGenerateResourcesFor)
		{
			if (cell->getHeight() > WATER_HEIGHT && cell->getHeight() <= GRASS_STONE_BORDER_HEIGHT)
			{
				// Cell is a grass cell, so it is eligible for being populated with a tree.
				std::unordered_set<Cell*> cellsWithinDistance;
				getCellsWithinDistance(cell, TREE_DENSITY, cellsWithinDistance);

				float max = 0;
				for (Cell* cell : cellsWithinDistance)
				{
					float noise = noiseMap[cell];
					if (noise > max)
						max = noise;
				}

				if (noiseMap[cell] == max)
					cell->setContent(new world::Tree());
			}
		}
	}

	void ResourceGenerator::getCellsWithinDistance(Cell* startingCell, unsigned int maxMoves, std::unordered_set<Cell*>& result)
	{
		if (result.find(startingCell) != result.end())
			return;

		result.insert(startingCell);

		if (maxMoves > 0)
			for (Cell* neighbor : startingCell->getNeighbors())
				getCellsWithinDistance(neighbor, maxMoves - 1, result);
	}
}
