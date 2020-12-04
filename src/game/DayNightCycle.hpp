#pragma once

#include <algorithm>
#include <glm/glm.hpp>

namespace game
{
	class DayNightCycle
	{
	public:
		DayNightCycle() : speed(0.0), time(0), sunDirection(glm::vec3(0, 1, 0)), sunColor(glm::vec3(1)) {}
		DayNightCycle(float _speed) : speed(_speed), time(0), sunDirection(glm::vec3(0, 1, 0)), sunColor(glm::vec3(1)) {}

		float speed;

		void update(float delta);

		float getTime() { return time; }

		glm::vec3 getSunDirection() { return sunDirection; }
		glm::vec3 getSunColor() { return sunColor; }

	private:
		float time;
		glm::vec3 sunDirection;
		glm::vec3 sunColor;
	};
}