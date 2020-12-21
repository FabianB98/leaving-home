#pragma once

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class AABB : public BoundingGeometry
	{
	public:
		AABB() : AABB(glm::vec3(0.0f), glm::vec3(0.0f)) {}

		AABB(glm::vec3 _min, glm::vec3 _max) : min(_min), max(_max) {}

		AABB(const std::vector<glm::vec3>& vertices);

		void fitToVertices(const std::vector<glm::vec3>& vertices);

		bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix);

	private:
		glm::vec3 min;
		glm::vec3 max;
	};
}
