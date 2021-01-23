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

	struct SpotLight
	{
		SpotLight(glm::vec3 _intensity, glm::vec3 _position, glm::vec3 _direction, float _fov)
			: intensity(_intensity), position(_position), direction(_direction), fov(_fov) {}

		glm::vec3 intensity;
		glm::vec3 position;
		glm::vec3 direction;
		float fov;

		// simplification of the point light method with c=l=0 and q=1
		float getRadius(float cutoff = 5.f / 255.f)
		{
			float iMax = fmaxf(fmaxf(intensity.r, intensity.g), intensity.b);
			float q = (1.0 - cosf(fov) + 0.5 * sinf(fov) * sinf(fov)) / 2;
			return sqrtf(q * iMax / cutoff) / q;
		}

		glm::vec3 packDirection()
		{
			return packDirection(direction);
		}

		// Stores the direction as two angles and the fov in a single vec3
		glm::vec3 packDirection(glm::vec3 dir)
		{
			float length = glm::length(dir);
			float theta = acosf(dir.z / length);
			float phi = atan2f(dir.y, dir.x);

			return glm::vec3(theta, phi, fov);
		}
	};
}