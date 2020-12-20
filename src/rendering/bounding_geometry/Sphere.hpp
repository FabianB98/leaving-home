#pragma once

#include <algorithm>
#include <random>
#include <unordered_map>
#include <vector>

#include <glm/gtx/norm.hpp>

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class Sphere : public BoundingGeometry
	{
	public:
		Sphere(glm::vec3 _center, float _radius) : center(_center), radius(_radius) {}

		Sphere(std::vector<glm::vec3> vertices);

		bool isInCameraFrustum(std::array<glm::vec4, 6> clippingPlanes, glm::mat4 modelMatrix);

	private:
		glm::vec3 center;
		float radius;

		std::pair<glm::vec3, float> smallestEnclosingBallWithPoints(
			std::unordered_map<size_t, glm::vec3>& P, 
			std::unordered_map<size_t, glm::vec3>& Q
		);
	};
}
