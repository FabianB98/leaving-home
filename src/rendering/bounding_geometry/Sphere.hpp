#pragma once

#include <algorithm>
#include <random>
#include <unordered_map>

#include <glm/gtx/norm.hpp>

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class Sphere : public BoundingGeometry
	{
	public:
		Sphere() : Sphere(glm::vec3(0.0f), 0.0f) {}

		Sphere(glm::vec3 _center, float _radius) : center(_center), radius(_radius) {}

		Sphere(const std::vector<glm::vec3>& vertices);

		void fitToVertices(const std::vector<glm::vec3>& vertices);

		bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix);

		glm::vec3 getCenter()
		{
			return center;
		}

		float getRadius()
		{
			return radius;
		}

	private:
		glm::vec3 center;
		float radius;

		glm::vec3 furthestPointAwayFrom(const std::vector<glm::vec3>& vertices, const glm::vec3& p);
	};
}
