#pragma once

#include <unordered_map>

#include "Chunk.hpp"
#include "PlanarGraph.hpp"

namespace std
{
	template <>
	struct hash<std::pair<int32_t, int32_t>>
	{
		std::size_t operator()(const std::pair<int32_t, int32_t>& pair) const
		{
			size_t res = 17;
			res = res * 31 + hash<int32_t>()(pair.first);
			res = res * 31 + hash<int32_t>()(pair.second);
			return res;
		}
	};
}

namespace game::world
{
	class World
	{
	public:
		World(size_t _worldSeed);

		~World();

		size_t getWorldSeed()
		{
			return worldSeed;
		}

		Chunk* getChunk(int32_t column, int32_t row);

		Chunk* generateChunk(int32_t column, int32_t row);

		const std::unordered_map<std::pair<int32_t, int32_t>, Chunk*> getChunks()
		{
			return chunks;
		}

		rendering::model::Mesh* getWaterMesh()
		{
			return waterMesh;
		}

	private:
		size_t worldSeed;

		std::unordered_map<std::pair<int32_t, int32_t>, Chunk*> chunks;
		PlanarGraph graph;

		rendering::model::Mesh* waterMesh;
	};
}
