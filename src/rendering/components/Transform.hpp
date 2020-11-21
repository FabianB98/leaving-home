#pragma once

#include <glm/glm.hpp>

namespace rendering::components
{
	class Transform
	{
	public:
		Transform(glm::mat4 _transform) : transform(_transform) {};

		glm::mat4 getTransform()
		{
			return transform;
		}

		void setTransform(glm::mat4 _transform)
		{
			transform = _transform;
		}

	private:
		glm::mat4 transform{ glm::mat4(1.f) };
	};
}