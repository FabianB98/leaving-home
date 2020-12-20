#pragma once

#include <vector>

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class AABB : public BoundingGeometry
	{
	public:
		AABB(glm::vec3 _min, glm::vec3 _max) : min(_min), max(_max) {}

		AABB(std::vector<glm::vec3> vertices);

		bool isInCameraFrustum(std::array<glm::vec4, 6> clippingPlanes, glm::mat4 modelMatrix);

	private:
		glm::vec3 min;
		glm::vec3 max;
	};
}
