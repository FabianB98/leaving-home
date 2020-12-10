#include "ResourceGenerator.hpp"

namespace game::world
{
	void ResourceGenerator::generateResources()
	{
		std::unordered_set<Cell*> cellsToCalculateNoiseFor;
		for (Cell* cell : chunk->getCells())
			getCellsWithinDistance(cell, TREE_DENSITY, cellsToCalculateNoiseFor);

		std::unordered_map<Cell*, float> noiseMap;
		for (Cell* cell : cellsToCalculateNoiseFor)
			noiseMap.insert(std::make_pair(cell, chunk->getHeightGenerator().getBlueNoise(cell->getPosition())));

		for (Cell* cell : chunk->getCells())
		{
			if (cell->getHeight() > WATER_HEIGHT && cell->getHeight() <= GRASS_STONE_BORDER_HEIGHT)
			{
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
