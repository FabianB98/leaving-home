#include "World.hpp"

#define WATER_RELATIVE_VERTEX_DENSITY 4

namespace game::world
{
	World::World(size_t _worldSeed) : worldSeed(_worldSeed)
	{
		int chunkSize = CHUNK_SIZE * WATER_RELATIVE_VERTEX_DENSITY;
		float cellSize = CELL_SIZE * (1.0f / (float)WATER_RELATIVE_VERTEX_DENSITY);

		Chunk waterChunk = Chunk(worldSeed, 0, 0, chunkSize, cellSize);
		waterMesh = waterChunk.generateWaterMesh();
	}

	World::~World()
	{
		for (auto& chunk : chunks)
			delete chunk.second;

		delete waterMesh;
	}

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
}
