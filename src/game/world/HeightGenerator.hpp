#pragma once

#include <glm/glm.hpp>
#include <FastNoiseLite.h>

#include "PlanarGraph.hpp"

#define HEIGHT_QUANTIZATION_STEP_SIZE 4.0f

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
	};
}
