#pragma once

namespace game::world
{
	// Constants related to the size of the world (size of chunks and individual cells).
	constexpr int CHUNK_SIZE = 5;
	constexpr float CELL_SIZE = 6.0f;

	// Constants related to the cluster relaxation.
	constexpr int CLUSTER_RELAXATION_ITERATIONS = 16;
	constexpr float CLUSTER_RELAXATION_UPDATE_WEIGHT = 1.0f;

	// Constants related to the height generation.
	constexpr float LANDSCAPE_SCALE = 0.125f;
	constexpr float HEIGHT_SCALE = 20.0f;
	constexpr float HEIGHT_REDISTRIBUTION_EXPONENT = 3.5f;
	constexpr float HEIGHT_QUANTIZATION_STEP_SIZE = 2.0f;

	// Constants related to the height of different landscape materials (water, grass, stone, snow, grass).
	constexpr float WATER_HEIGHT = 2.5f * HEIGHT_QUANTIZATION_STEP_SIZE;
	constexpr int WATER_RELATIVE_VERTEX_DENSITY = 4;
	constexpr float SAND_GRASS_BORDER_HEIGHT = 2.0f * HEIGHT_QUANTIZATION_STEP_SIZE;
	constexpr float SAND_GRASS_BORDER_DEVIATION = 5.0f;
	constexpr float GRASS_STONE_BORDER_HEIGHT = 24.0f;
	constexpr float GRASS_STONE_BORDER_DEVIATION = 15.0f;
	constexpr float STONE_SNOW_BORDER_HEIGHT = 75.0f;
	constexpr float STONE_SNOW_BORDER_DEVIATION = 30.0f;

	// Constants related to the generation of resources.
	constexpr bool GENERATE_RESOURCES = true;
	constexpr int TREE_DENSITY = 1;

	// Constants related to the mesh generation of chunks.
	constexpr bool ADD_TOPOLOGY_MESH = false;
	constexpr bool ADD_LANDSCAPE_MESH = true;
	constexpr bool ADD_WATER_MESH = true;
	constexpr unsigned int CELL_ID_ATTRIBUTE_LOCATION = 14;
}
