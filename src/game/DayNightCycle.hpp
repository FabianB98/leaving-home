#pragma once

#include <algorithm>
#include <glm/glm.hpp>

namespace game
{
	class DayNightCycle
	{
	public:
		DayNightCycle() : speed(0.0), time(0), sunDirection(glm::vec3(0, 1, 0)), sunColor(glm::vec3(1)), shadows(1.f) {}
		DayNightCycle(float _speed) : speed(_speed), time(0), sunDirection(glm::vec3(0, 1, 0)), sunColor(glm::vec3(1)), shadows(1.f) {}

		float speed;

		void update(float delta);

		void setTime(float _time);
		float getTime() { return time; }

		float getShadows() { return shadows; }
		glm::vec3 getSunDirection() { return sunDirection; }
		glm::vec3 getSunColor() { return sunColor; }
		glm::vec3 getStarlight() { return starlight; };

	private:
		float time;
		float shadows;
		glm::vec3 sunDirection;
		glm::vec3 sunColor;
		glm::vec3 starlight;
	};
}