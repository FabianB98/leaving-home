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
		brightness = smoothstep(-half - dd*0.25f, -half + dd, time) * smoothstep(half + dd*0.25f, half - dd, time);
		float dawn = smoothstep(-half - 2*dd, -half + 2*dd, time) * smoothstep(half + 2*dd, half - 2*dd, time);

		shadows = smoothstep(-half, -half + dd * 0.5f, time) * smoothstep(half, half - dd * 0.5f, time);

		sunColor = 2 * brightness * (dawn * glm::vec3(1) + (1.f - dawn) * glm::vec3(1, .25f, 0));
		sunDirection.y = cos(time);
		sunDirection.z = sin(time);

		starlight = (1.f - brightness) * glm::vec3(0.3);
	}

	void DayNightCycle::setTime(float _time)
	{
		time = std::max(std::min(_time, (float) M_PI), (float) -M_PI);
	}
}