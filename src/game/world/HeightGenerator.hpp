#pragma once

#include <math.h>

#include <glm/glm.hpp>
#include <FastNoiseLite.h>

#include "Constants.hpp"
#include "PlanarGraph.hpp"
#include "../../util/MathUtil.hpp"

namespace game::world
{
	class HeightGenerator
	{
	public:
		HeightGenerator(size_t seed);

		float getNoise(glm::vec2 pos)
		{
			return getNoise(pos.x, pos.y);
		}

		float getNoise(float x, float y);

		float getBlueNoise(glm::vec2 pos)
		{
			return getBlueNoise(pos.x, pos.y);
		}

		float getBlueNoise(float x, float y);

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

		float getOctave(float x, float y, float octave);
	};
}
