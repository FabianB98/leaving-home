#include "World.hpp"

namespace game::world
{
	Chunk* World::getChunk(int32_t column, int32_t row)
	{
		auto& chunk = chunks.find(std::make_pair(column, row));
		if (chunk != chunks.end())
			return chunk->second;
		else
			return nullptr;
	}

	Chunk* World::generateChunk(int32_t column, int32_t row)
	{
		Chunk* chunk = getChunk(column, row);
		if (chunk != nullptr)
			return chunk;

		chunk = new Chunk(worldSeed, column, row);
		chunks.insert(std::make_pair(std::make_pair(column, row), chunk));

		Chunk* neighbors[6]{
				getChunk(column + 1, row - 1),	// Neighbor chunk to the diagonal up right
				getChunk(column + 1, row + 0),	// Neighbor chunk to the right
				getChunk(column + 0, row + 1),	// Neighbor chunk to the diagonal down right
				getChunk(column - 1, row + 1),	// Neighbor chunk to the diagonal down left
				getChunk(column - 1, row + 0),	// Neighbor chunk to the left
				getChunk(column + 0, row - 1)	// Neighbor chunk to the diagonal up left
		};
		chunk->generateChunkTopology(neighbors, &graph);

		return chunk;
	}

	World::~World()
	{
		for (auto& chunk : chunks)
			delete chunk.second;
	}
}
