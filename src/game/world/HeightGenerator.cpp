#include "HeightGenerator.hpp"

namespace game::world
{
	HeightGenerator::HeightGenerator(size_t seed)
	{
		heightNoise.SetSeed(seed);
		heightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	}

	float HeightGenerator::getHeight(float x, float y)
	{
		return 10.0f * heightNoise.GetNoise(x, y);
	}

	float HeightGenerator::quantizeHeight(float height)
	{
		return fastRound(height / HEIGHT_QUANTIZATION_STEP_SIZE) * HEIGHT_QUANTIZATION_STEP_SIZE;
	}
}
