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
		PointLight(glm::vec3 _intensity, glm::vec3 _position, glm::vec3 _attenuation)
			: intensity(_intensity), position(_position), attenuation(_attenuation) {}

		glm::vec3 intensity;
		glm::vec3 position;
		glm::vec3 attenuation;

		// https://learnopengl.com/Advanced-Lighting/Deferred-Shading
		float getRadius(float cutoff = 5.f / 255.f)
		{
			float c = attenuation.x;	// constant
			float l = attenuation.y;	// linear
			float q = attenuation.z;	// quadratic
			float iMax = fmaxf(fmaxf(intensity.r, intensity.g), intensity.b);

			return (-l + sqrtf(l * l - 4 * q * (c - iMax / cutoff))) / (2.f * q);
		}
	};
}