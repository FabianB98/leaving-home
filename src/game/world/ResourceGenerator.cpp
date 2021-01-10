#include "ResourceGenerator.hpp"

namespace game::world
{
	void ResourceGenerator::generateResources(
		unsigned int density, 
		std::function<bool(Cell*)> eligible, 
		std::function<world::Resource*()> create
	) {
		// Determine all cells for which we need to generate the resources for. A cell of the current chunk is eligible
		// for being populated by some resource if it and all of its neighboring cells are relaxed and if it is not yet
		// populated with something.
		std::unordered_set<Cell*> cellsToGenerateResourcesFor;
		for (Cell* cell : chunk->getCellsAndCellsAlongChunkBorder())
		{
			bool allNeighborsRelaxed = true;
			for (Cell* neighbor : cell->getNeighbors())
				if (!neighbor->isRelaxed())
				{
					allNeighborsRelaxed = false;
					break;
				}

			bool eligibleForResources = cell->isRelaxed() && allNeighborsRelaxed && cell->getContent() == nullptr;
			if (eligibleForResources && eligible(cell))
				cellsToGenerateResourcesFor.insert(cell);
		}

		// Determine the noise for all eligible cells of the chunk and all neighboring cells that can be reached within
		// TREE_DENSITY moves.
		std::unordered_set<Cell*> cellsToCalculateNoiseFor;
		for (Cell* cell : cellsToGenerateResourcesFor)
			getCellsWithinDistance(cell, density, cellsToCalculateNoiseFor);

		std::unordered_map<Cell*, float> noiseMap;
		for (Cell* cell : cellsToCalculateNoiseFor)
			noiseMap.insert(std::make_pair(cell, chunk->getHeightGenerator().getBlueNoise(cell->getRelaxedPosition())));

		// Actually generate resources for all eligible cells.
		for (Cell* cell : cellsToGenerateResourcesFor)
		{
			std::unordered_set<Cell*> cellsWithinDistance;
			getCellsWithinDistance(cell, density, cellsWithinDistance);

			float max = 0;
			for (Cell* cell : cellsWithinDistance)
			{
				float noise = noiseMap[cell];
				if (noise > max)
					max = noise;
			}

			if (noiseMap[cell] == max && cell->getContent() == nullptr)
				cell->setContent(create());
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
