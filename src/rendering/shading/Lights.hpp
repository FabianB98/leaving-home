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
	class DirectionalLight
	{
	public:
		DirectionalLight(glm::vec3 _intensity, glm::vec3 _direction)
			: intensity(_intensity), direction(_direction) {};

		glm::vec3 getIntensity() const;

		void setIntensity(glm::vec3& _intensity);

		glm::vec3 getDirection() const;

		void setDirection(glm::vec3& _direction);

	private:
		glm::vec3 intensity;
		glm::vec3 direction;
	};

	class PointLight
	{
	public:
		PointLight(glm::vec3 _intensity, glm::vec3 _position)
			: intensity(_intensity), position(_position) {};

		glm::vec3 getIntensity() const;

		void setIntensity(glm::vec3& _intensity);

		glm::vec3 getPosition() const;

		void setPosition(glm::vec3& _position);

	private:
		glm::vec3 intensity;
		glm::vec3 position;
	};
}