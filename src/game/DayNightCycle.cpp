#include "DayNightCycle.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace game
{
	// a classic smoothstep implementation
	float smoothstep(float t0, float t1, float x) {
		x = std::clamp((x - t0) / (t1 - t0), 0.f, 1.f);
		return x * x * (3 - 2 * x);
	}

	void DayNightCycle::update(float delta)
	{
		time += speed * delta;
		if (time >= M_PI) time -= 2 * M_PI;

		float half = M_PI * .5f;
		float dd = M_PI * .1f;
		float brightness = smoothstep(-half - dd, -half, time) * smoothstep(half + dd, half, time);
		float dawn = smoothstep(-half - 2*dd, -half + 2*dd, time) * smoothstep(half + 2*dd, half - 2*dd, time);

		sunColor = 2 * brightness * (dawn * glm::vec3(1) + (1.f - dawn) * glm::vec3(1, .25f, 0));
		sunDirection.y = cos(time);
		sunDirection.z = sin(time);
	}
}