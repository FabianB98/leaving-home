#include "AABB.hpp"

namespace rendering::bounding_geometry
{
	AABB::AABB(const std::vector<glm::vec3>& vertices) : min(glm::vec3(0.0f)), max(glm::vec3(0.0f))
	{
		fitToVertices(vertices);
	}

	void AABB::fitToVertices(const std::vector<glm::vec3>& vertices)
	{
		min = vertices[0];
		max = vertices[0];

		for (size_t i = 1; i < vertices.size(); i++)
		{
			min = glm::min(min, vertices[i]);
			max = glm::max(max, vertices[i]);
		}
	}

	bool AABB::isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix)
	{
		// Translate, rotate and scale the AABB according to the model matrix.
		glm::vec4 cornersHomogeneous[8];
		cornersHomogeneous[0] = modelMatrix * glm::vec4(min.x, min.y, min.z, 1);
		cornersHomogeneous[1] = modelMatrix * glm::vec4(min.x, min.y, max.z, 1);
		cornersHomogeneous[2] = modelMatrix * glm::vec4(min.x, max.y, min.z, 1);
		cornersHomogeneous[3] = modelMatrix * glm::vec4(min.x, max.y, max.z, 1);
		cornersHomogeneous[4] = modelMatrix * glm::vec4(max.x, min.y, min.z, 1);
		cornersHomogeneous[5] = modelMatrix * glm::vec4(max.x, min.y, max.z, 1);
		cornersHomogeneous[6] = modelMatrix * glm::vec4(max.x, max.y, min.z, 1);
		cornersHomogeneous[7] = modelMatrix * glm::vec4(max.x, max.y, max.z, 1);

		glm::vec3 corner = glm::vec3(cornersHomogeneous[0]) / cornersHomogeneous[0].w;
		glm::vec3 rotatedMin = corner;
		glm::vec3 rotatedMax = corner;
		for (int i = 1; i < 8; i++)
		{
			corner = glm::vec3(cornersHomogeneous[i]) / cornersHomogeneous[i].w;
			rotatedMin = glm::min(rotatedMin, corner);
			rotatedMax = glm::max(rotatedMax, corner);
		}

		// AABB frustum intersection test as described in 
		// https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
		bool inFrustum = true;
		for (int i = 0; i < 6; i++) {
			glm::vec3 clippingPlane = glm::vec3(clippingPlanes[i]);
			glm::vec3 minTimesPlane = rotatedMin * clippingPlane;
			glm::vec3 maxTimesPlane = rotatedMax * clippingPlane;
			glm::vec3 componentwiseMax = glm::max(minTimesPlane, maxTimesPlane);

			float distanceToPlane = componentwiseMax.x + componentwiseMax.y + componentwiseMax.z + clippingPlanes[i].w;
			inFrustum &= distanceToPlane > 0;
		}
		return inFrustum;
	}
}
