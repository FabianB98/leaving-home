#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

// OpenGL related headers
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace rendering
{
	struct DirectionalLight
	{
		glm::vec3 intensity;
		glm::vec3 direction;
	};

	struct PointLight
	{
		glm::vec3 intensity;
		glm::vec3 position;
	};
}