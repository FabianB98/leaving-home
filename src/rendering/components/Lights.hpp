#pragma once

#include <glm/glm.hpp>

namespace rendering::components
{
	struct DirectionalLight
	{
		DirectionalLight(glm::vec3 _intensity, glm::vec3 _direction) : intensity(_intensity), direction(_direction) {}

		glm::vec3 intensity;
		glm::vec3 direction;
	};

	struct PointLight
	{
		PointLight(glm::vec3 _intensity, glm::vec3 _position) : intensity(_intensity), position(_position) {}

		glm::vec3 intensity;
		glm::vec3 position;
	};
}