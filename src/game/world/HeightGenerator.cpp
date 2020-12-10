#include "HeightGenerator.hpp"

namespace game::world
{
	HeightGenerator::HeightGenerator(size_t seed)
	{
		heightNoise.SetSeed(seed);
		heightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	}

	float HeightGenerator::getNoise(float x, float y)
	{
		// FastNoiseLite generates noise values between -1.0 and +1.0, but we want noise values between 0.0 and 1.0.
		return (heightNoise.GetNoise(x, y) + 1.0f) / 2.0f;
	}

	float HeightGenerator::getOctave(float x, float y, float octave)
	{
		return getNoise(x * octave, y * octave) / octave;
	}

	float HeightGenerator::getBlueNoise(float x, float y)
	{
		return getNoise(50.0f * x, 50.0f * y);
	}

	float HeightGenerator::getHeight(float x, float y)
	{
		// Apply landscape scale (i.e. stretch or squeeze the landscape)
		float xScaled = LANDSCAPE_SCALE * x;
		float yScaled = LANDSCAPE_SCALE * y;

		// Add 5 octaves to get the initial height.
		float unscaledHeight = 
			getOctave(xScaled, yScaled, 1) + 
			getOctave(xScaled, yScaled, 2) + 
			getOctave(xScaled, yScaled, 4) +
			getOctave(xScaled, yScaled, 8) +
			getOctave(xScaled, yScaled, 16);

		// Redistribute the height (i.e. add more valleys).
		unscaledHeight = powf(unscaledHeight, HEIGHT_REDISTRIBUTION_EXPONENT);

		// Apply height scale.
		return HEIGHT_SCALE * unscaledHeight;
	}

	float HeightGenerator::quantizeHeight(float height)
	{
		return fastRound(height / HEIGHT_QUANTIZATION_STEP_SIZE) * HEIGHT_QUANTIZATION_STEP_SIZE;
	}
}
