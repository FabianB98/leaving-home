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
		for (auto& chunk : allChunks)
			delete chunk.second;
	}

	Chunk* World::getChunkFromAllChunks(int32_t column, int32_t row)
	{
		auto& chunk = allChunks.find(std::make_pair(column, row));
		if (chunk != allChunks.end())
			return chunk->second;
		else
			return nullptr;
	}

	Chunk* World::getOrGenerateChunkFromAllChunks(int32_t column, int32_t row)
	{
		Chunk* chunk = getChunkFromAllChunks(column, row);
		if (chunk != nullptr)
			return chunk;

		// Create a new chunk and generate its topology (i.e. cells and their neighborhood).
		chunk = new Chunk(worldSeed, column, row, heightGenerator, registry, terrainShader, waterShader);
		allChunks.insert(std::make_pair(std::make_pair(column, row), chunk));

		Chunk* neighbors[6]{
				getChunkFromAllChunks(column + 1, row - 1),	// Neighbor chunk to the diagonal up right
				getChunkFromAllChunks(column + 1, row + 0),	// Neighbor chunk to the right
				getChunkFromAllChunks(column + 0, row + 1),	// Neighbor chunk to the diagonal down right
				getChunkFromAllChunks(column - 1, row + 1),	// Neighbor chunk to the diagonal down left
				getChunkFromAllChunks(column - 1, row + 0),	// Neighbor chunk to the left
				getChunkFromAllChunks(column + 0, row - 1)	// Neighbor chunk to the diagonal up left
		};
		chunk->generateChunkTopology(neighbors, &graph);

		// Relax the positions of the cells within the chunk, but keep the positions of the cells along the chunk's
		// border as they are.
		ChunkCluster cluster = ChunkCluster(std::vector<Chunk*>{chunk}, true);
		cluster.relax();

		for (auto& cell : chunk->getCells())
			cell->node->setPosition(cluster.getRelaxedPosition(cell));

		return chunk;
	}

	Chunk* World::getChunk(int32_t column, int32_t row)
	{
		auto& chunk = relaxedChunks.find(std::make_pair(column, row));
		if (chunk != relaxedChunks.end())
			return chunk->second;
		else
			return nullptr;
	}

	Chunk* World::generateChunk(int32_t column, int32_t row)
	{
		Chunk* chunk = getChunk(column, row);
		if (chunk != nullptr)
			return chunk;

		// TODO: This is just some temporary test code to check whether the relaxation of a single cluster works, and
		// not the correct way of relaxing chunks. Finish implementing chunk relaxation and change everything between
		// this comment and the end of this method to use the correct chunk relaxation.
		chunk = getOrGenerateChunkFromAllChunks(column, row);
		Chunk* chunk1 = getOrGenerateChunkFromAllChunks(column, row - 1);
		Chunk* chunk2 = getOrGenerateChunkFromAllChunks(column + 1, row - 1);

		ChunkCluster cluster = ChunkCluster(std::vector<Chunk*>{chunk, chunk1, chunk2}, false);
		cluster.relax();

		for (auto& cell : chunk->getCells())
			cell->node->setPosition(cluster.getRelaxedPosition(cell));

		for (auto& cell : chunk1->getCells())
			cell->node->setPosition(cluster.getRelaxedPosition(cell));

		for (auto& cell : chunk2->getCells())
			cell->node->setPosition(cluster.getRelaxedPosition(cell));

		relaxedChunks.insert(std::make_pair(std::make_pair(column, row), chunk));
		relaxedChunks.insert(std::make_pair(std::make_pair(column, row), chunk1));
		relaxedChunks.insert(std::make_pair(std::make_pair(column, row), chunk2));

		chunk->addedToWorld();
		chunk1->addedToWorld();
		chunk2->addedToWorld();

		ResourceGenerator resourceGenerator = ResourceGenerator(chunk);
		resourceGenerator.generateResources();

		resourceGenerator = ResourceGenerator(chunk1);
		resourceGenerator.generateResources();

		resourceGenerator = ResourceGenerator(chunk2);
		resourceGenerator.generateResources();

		return chunk;
	}
}
