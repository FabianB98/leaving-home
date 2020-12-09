#pragma once

#include <math.h>

#include <glm/glm.hpp>
#include <FastNoiseLite.h>

#include "PlanarGraph.hpp"

constexpr float LANDSCAPE_SCALE = 0.25f;
constexpr float HEIGHT_SCALE = 20.0f;

constexpr float HEIGHT_REDISTRIBUTION_EXPONENT = 1.8f;

constexpr float HEIGHT_QUANTIZATION_STEP_SIZE = 4.0f;

namespace game::world
{
	class HeightGenerator
	{
	public:
		HeightGenerator(size_t seed);

		float getHeight(glm::vec2 pos)
		{
			return getHeight(pos.x, pos.y);
		}

		float getHeight(float x, float y);

		float getHeightQuantized(glm::vec2 pos)
		{
			return getHeightQuantized(pos.x, pos.y);
		}

		float getHeightQuantized(float x, float y)
		{
			return quantizeHeight(getHeight(x, y));
		}

		float quantizeHeight(float height);

	private:
		FastNoiseLite heightNoise;

		float getNoise(float x, float y);

		float getOctave(float x, float y, float octave);
	};
}
