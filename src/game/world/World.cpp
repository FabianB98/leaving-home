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
		waterShader(_waterShader),
		chunksToGenerate(moodycamel::ReaderWriterQueue<std::pair<int32_t, int32_t>>(100)),
		generatedChunks(moodycamel::ReaderWriterQueue<Chunk*>(100))
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

		worldGenerationThreadStopFuture = worldGenerationThreadStopSignal.get_future();
		worldGenerationThread = std::thread(&World::worldGenerationThreadLoop, this);
	}

	World::~World()
	{
		stopWorldGenerationThread();

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

	Chunk* World::getOrGenerateChunkFromAllChunks(int32_t column, int32_t row, bool& needsToBeRelaxed)
	{
		Chunk* chunk = getChunkFromAllChunks(column, row);
		if (chunk != nullptr)
		{
			needsToBeRelaxed = false;
			return chunk;
		}

		// Create a new chunk and generate its topology (i.e. cells and their neighborhood).
		chunk = new Chunk(worldSeed, column, row, heightGenerator, registry, terrainShader, waterShader);
		allChunks.insert(std::make_pair(std::make_pair(column, row), chunk));

		std::array<Chunk*, 6> neighbors{
			getChunkFromAllChunks(column + 1, row - 1),	// Neighbor chunk to the diagonal up right
			getChunkFromAllChunks(column + 1, row + 0),	// Neighbor chunk to the right
			getChunkFromAllChunks(column + 0, row + 1),	// Neighbor chunk to the diagonal down right
			getChunkFromAllChunks(column - 1, row + 1),	// Neighbor chunk to the diagonal down left
			getChunkFromAllChunks(column - 1, row + 0),	// Neighbor chunk to the left
			getChunkFromAllChunks(column + 0, row - 1)	// Neighbor chunk to the diagonal up left
		};
		chunk->generateChunkTopology(neighbors, &graph);

		needsToBeRelaxed = true;
		return chunk;
	}

	ChunkCluster* World::getOrGenerateChunkCluster(Chunk* chunkA, Chunk* chunkB, Chunk* chunkC, bool& needsToBeRelaxed)
	{
		// Check whether we have already relaxed this cluster.
		std::vector<Chunk*> chunks = std::vector<Chunk*>{ chunkA, chunkB, chunkC };
		ChunkClusterIdentifier identifier = ChunkClusterIdentifier(chunks);
		
		auto& identifiedCluster = chunkClusters.find(identifier);
		if (identifiedCluster != chunkClusters.end())
		{
			needsToBeRelaxed = false;
			return identifiedCluster->second;
		}
		
		// There is no cluster for the chunks yet. Create a new cluster for the chunks.
		ChunkCluster* cluster = new ChunkCluster(chunks, false);
		chunkClusters.insert(std::make_pair(identifier, cluster));

		needsToBeRelaxed = true;
		return cluster;
	}

	Chunk* World::getChunkByChunkId(uint16_t chunkId)
	{
		auto& chunks = relaxedChunksById.find(chunkId);
		if (chunks != relaxedChunksById.end())
			// TODO: So far this function assumes that there is only one chunk with the given ID. However, this
			// assumption will break as soon as one axis (row or column) spans more than 128 chunks. Therefore, a better
			// solution needs to be implemented before the world contains more than 128 along one axis.
			return chunks->second[0];
		else
			return nullptr;
	}

	Chunk* World::getChunkByCompleteCellId(uint32_t completeCellId)
	{
		uint16_t chunkId = (completeCellId >> 10) & 0x3FFF;
		return getChunkByChunkId(chunkId);
	}

	Chunk* World::getChunk(int32_t column, int32_t row)
	{
		auto& chunk = relaxedChunks.find(std::make_pair(column, row));
		if (chunk != relaxedChunks.end())
			return chunk->second;
		else
			return nullptr;
	}

	void World::generateChunk(int32_t column, int32_t row)
	{
		if (getChunk(column, row) != nullptr)
			return;

		chunksToGenerate.enqueue(std::make_pair(column, row));
	}

	void World::_generateChunk(int32_t column, int32_t row)
	{
		if (getChunk(column, row) != nullptr)
			return;

		std::cout << "Generating chunk at (" << column << "|" << row << ")" << std::endl;

		// Chunk is not yet (fully) generated. Get (or generate if not yet generated) the unrelaxed chunk and its six
		// neighboring chunks.
		bool chunksToRelax[7];
		Chunk* chunks[7]{
			getOrGenerateChunkFromAllChunks(column + 0, row + 0, chunksToRelax[0]),	// Chunk to generate
			getOrGenerateChunkFromAllChunks(column + 1, row - 1, chunksToRelax[1]),	// Neighbor to the upper right
			getOrGenerateChunkFromAllChunks(column + 1, row + 0, chunksToRelax[2]),	// Neighbor to the right
			getOrGenerateChunkFromAllChunks(column + 0, row + 1, chunksToRelax[3]),	// Neighbor to the lower right
			getOrGenerateChunkFromAllChunks(column - 1, row + 1, chunksToRelax[4]),	// Neighbor to the lower left
			getOrGenerateChunkFromAllChunks(column - 1, row + 0, chunksToRelax[5]),	// Neighbor to the left
			getOrGenerateChunkFromAllChunks(column + 0, row - 1, chunksToRelax[6])	// Neighbor to the upper left
		};

		// Relax all chunks which were not yet relaxed individually in parallel. This is the case for all chunks that
		// had to be generated by getOrGenerateChunkFromAllChunks as they didn't exist before.
		static const std::vector<int> chunkCount{ 0, 1, 2, 3, 4, 5, 6 };
		std::for_each(std::execution::par_unseq, std::begin(chunkCount), std::end(chunkCount), [&](int i) {
			if (chunksToRelax[i])
			{
				// Relax the positions of the cells within the chunk, but keep the positions of the cells along the
				// chunk's border as they are.
				ChunkCluster cluster = ChunkCluster(std::vector<Chunk*>{chunks[i]}, true);
				cluster.relax();

				for (auto& cell : chunks[i]->getCells())
					cell.second->node->setPosition(cluster.getRelaxedPosition(cell.second));
			}
		});

		// Get (or generate if not yet generated) all six clusters around the chunk. A cluster is defined as the current
		// chunk and two adjacent neighbor chunks such that the three chunks share a common corner.
		bool clustersToRelax[6];
		std::array<ChunkCluster*, 6> clusters{
			getOrGenerateChunkCluster(chunks[0], chunks[6], chunks[1], clustersToRelax[0]),	// Upper corner
			getOrGenerateChunkCluster(chunks[0], chunks[1], chunks[2], clustersToRelax[1]),	// Upper right corner
			getOrGenerateChunkCluster(chunks[0], chunks[2], chunks[3], clustersToRelax[2]),	// Lower right corner
			getOrGenerateChunkCluster(chunks[0], chunks[3], chunks[4], clustersToRelax[3]),	// Lower corner
			getOrGenerateChunkCluster(chunks[0], chunks[4], chunks[5], clustersToRelax[4]),	// Lower left corner
			getOrGenerateChunkCluster(chunks[0], chunks[5], chunks[6], clustersToRelax[5]),	// Upper left corner
		};

		// Relax all clusters which were not yet relaxed in parallel. This the case for all clusters that had to be
		// generated by getOrGenerateChunkCluster as they didn't exist before.
		static const std::vector<int> clusterCount{ 0, 1, 2, 3, 4, 5 };
		std::for_each(std::execution::par_unseq, std::begin(clusterCount), std::end(clusterCount), [&](int i) {
			if (clustersToRelax[i])
				clusters[i]->relax();
		});

		ChunkCluster::updateChunkCells(chunks[0], clusters);

		relaxedChunks.insert(std::make_pair(std::make_pair(column, row), chunks[0]));
		relaxedChunksById[chunks[0]->getChunkId()].push_back(chunks[0]);

		generatedChunks.enqueue(chunks[0]);
	}

	void World::worldGenerationThreadLoop()
	{
		std::cout << "Started world generation thread!" << std::endl;

		std::pair<int32_t, int32_t> nextChunkToGenerate;
		while (worldGenerationThreadStopFuture.wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout)
		{
			while (chunksToGenerate.try_dequeue(nextChunkToGenerate))
			{
				_generateChunk(nextChunkToGenerate.first, nextChunkToGenerate.second);

				if (worldGenerationThreadStopFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::timeout)
					break;
			}
		}

		std::cout << "Stopped world generation thread!" << std::endl;
	}

	void World::update()
	{
		// In order to not hinder the rendering from continuing, at most one chunk is added per frame.
		Chunk* nextChunkToAdd;
		if (generatedChunks.try_dequeue(nextChunkToAdd))
		{
			nextChunkToAdd->addedToWorld();

			if (GENERATE_RESOURCES)
			{
				ResourceGenerator resourceGenerator = ResourceGenerator(nextChunkToAdd);
				// generate trees
				resourceGenerator.generateResources(TREE_DENSITY, 
					[](auto* cell) {
						return cell->getHeight() > WATER_HEIGHT && cell->getCellType() == CellType::GRASS;
					}, 
					[]() { return new Tree(); });
				// generate rocks
				resourceGenerator.generateResources(ROCK_DENSITY,
					[](auto* cell) {
						return cell->getHeight() > WATER_HEIGHT && cell->getCellType() == CellType::STONE;
					},
					[]() { return new Rock(); });
			}
		}

		// Update all dirty CellContents.
		registry.view<CellContentUpdate>().each([&](const auto entity, CellContentUpdate& cellContentUpdate) {
			cellContentUpdate.cellContent->update();
			registry.remove<CellContentUpdate>(entity);
		});

		// Update all dirty Chunks.
		registry.view<ChunkUpdate>().each([&](const auto entity, ChunkUpdate& chunkUpdate) {
			chunkUpdate.chunk->update();
			registry.remove<ChunkUpdate>(entity);
		});
	}

	void World::stopWorldGenerationThread()
	{
		worldGenerationThreadStopSignal.set_value();
		worldGenerationThread.join();
	}
}
