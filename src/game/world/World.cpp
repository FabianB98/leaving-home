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

		for (auto& cluster : chunkClusters)
			delete cluster.second;
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

		std::array<Chunk*, 6> neighbors = getNeighborsFromAllChunks(column, row);
		chunk->generateChunkTopology(neighbors, &graph);

		// Relax the positions of the cells within the chunk, but keep the positions of the cells along the chunk's
		// border as they are.
		ChunkCluster cluster = ChunkCluster(std::vector<Chunk*>{chunk}, true);
		cluster.relax();

		for (auto& cell : chunk->getCells())
			cell->node->setPosition(cluster.getRelaxedPosition(cell));

		return chunk;
	}

	std::array<Chunk*, 6> World::getNeighborsFromAllChunks(int32_t column, int32_t row)
	{
		return std::array<Chunk*, 6>{
				getChunkFromAllChunks(column + 1, row - 1),	// Neighbor chunk to the diagonal up right
				getChunkFromAllChunks(column + 1, row + 0),	// Neighbor chunk to the right
				getChunkFromAllChunks(column + 0, row + 1),	// Neighbor chunk to the diagonal down right
				getChunkFromAllChunks(column - 1, row + 1),	// Neighbor chunk to the diagonal down left
				getChunkFromAllChunks(column - 1, row + 0),	// Neighbor chunk to the left
				getChunkFromAllChunks(column + 0, row - 1)	// Neighbor chunk to the diagonal up left
		};
	}

	std::array<Chunk*, 6> World::getOrGenerateNeighborsFromAllChunks(int32_t column, int32_t row)
	{
		return std::array<Chunk*, 6>{
			getOrGenerateChunkFromAllChunks(column + 1, row - 1),	// Neighbor chunk to the diagonal up right
			getOrGenerateChunkFromAllChunks(column + 1, row + 0),	// Neighbor chunk to the right
			getOrGenerateChunkFromAllChunks(column + 0, row + 1),	// Neighbor chunk to the diagonal down right
			getOrGenerateChunkFromAllChunks(column - 1, row + 1),	// Neighbor chunk to the diagonal down left
			getOrGenerateChunkFromAllChunks(column - 1, row + 0),	// Neighbor chunk to the left
			getOrGenerateChunkFromAllChunks(column + 0, row - 1)	// Neighbor chunk to the diagonal up left
		};
	}

	ChunkCluster* World::getOrGenerateChunkCluster(Chunk* chunkA, Chunk* chunkB, Chunk* chunkC)
	{
		// Check whether we have already relaxed this cluster.
		std::vector<Chunk*> chunks = std::vector<Chunk*>{ chunkA, chunkB, chunkC };
		ChunkClusterIdentifier identifier = ChunkClusterIdentifier(chunks);
		
		auto& identifiedCluster = chunkClusters.find(identifier);
		if (identifiedCluster != chunkClusters.end())
			return identifiedCluster->second;
		
		// There is no cluster for the chunks yet. Create a new cluster for the chunks and relax it.
		ChunkCluster* cluster = new ChunkCluster(chunks, false);
		chunkClusters.insert(std::make_pair(identifier, cluster));

		cluster->relax();

		return cluster;
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

		std::cout << "Generating chunk at (" << column << "|" << row << ")" << std::endl;

		// Chunk is not yet (fully) generated. Get (or generate if not yet generated) the unrelaxed chunk and its six
		// neighboring chunks.
		chunk = getOrGenerateChunkFromAllChunks(column, row);
		std::array<Chunk*, 6> neighbors = getOrGenerateNeighborsFromAllChunks(column, row);

		// Get (or generate and relax if not yet generated) all six clusters around the chunk. A cluster is defined as
		// the current chunk and two adjacent neighbor chunks such that the three chunks share a common corner. After
		// each cluster was relaxed, the positions of the cells within the chunk need to be updated accordingly.
		std::array<ChunkCluster*, 6> clusters{
			getOrGenerateChunkCluster(chunk, neighbors[5], neighbors[0]),	// Cluster on the upper corner
			getOrGenerateChunkCluster(chunk, neighbors[0], neighbors[1]),	// Cluster on the upper right corner
			getOrGenerateChunkCluster(chunk, neighbors[1], neighbors[2]),	// Cluster on the lower right corner
			getOrGenerateChunkCluster(chunk, neighbors[2], neighbors[3]),	// Cluster on the lower corner
			getOrGenerateChunkCluster(chunk, neighbors[3], neighbors[4]),	// Cluster on the lower left corner
			getOrGenerateChunkCluster(chunk, neighbors[4], neighbors[5]),	// Cluster on the upper left corner
		};
		ChunkCluster::updateChunkCells(chunk, clusters);

		relaxedChunks.insert(std::make_pair(std::make_pair(column, row), chunk));
		chunk->addedToWorld();

		if (GENERATE_RESOURCES) 
		{
			ResourceGenerator resourceGenerator = ResourceGenerator(chunk);
			resourceGenerator.generateResources();
		}

		return chunk;
	}
}
