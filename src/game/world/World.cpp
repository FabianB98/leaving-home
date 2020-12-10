#include "World.hpp"

namespace game::world
{
	World::World(
		size_t _worldSeed,
		entt::registry& _registry,
		rendering::shading::Shader* _terrainShader,
		rendering::shading::Shader* _waterShader
	) :
		worldSeed(_worldSeed),
		heightGenerator(HeightGenerator(worldSeed)),
		registry(_registry),
		terrainShader(_terrainShader),
		waterShader(_waterShader)
	{
		int chunkSize = CHUNK_SIZE * WATER_RELATIVE_VERTEX_DENSITY;
		float cellSize = CELL_SIZE * (1.0f / (float)WATER_RELATIVE_VERTEX_DENSITY);

		Chunk waterChunk = Chunk(
			worldSeed, 
			0, 
			0, 
			heightGenerator, 
			registry, 
			terrainShader, 
			waterShader, 
			chunkSize, 
			cellSize
		);
		waterChunk.generateWaterMesh();
	}

	World::~World()
	{
		for (auto& chunk : chunks)
			delete chunk.second;
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

		chunk = new Chunk(worldSeed, column, row, heightGenerator, registry, terrainShader, waterShader);
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

		ResourceGenerator resourceGenerator = ResourceGenerator(chunk);
		resourceGenerator.generateResources();

		return chunk;
	}
}
