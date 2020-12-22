#include "AABB.hpp"

namespace rendering::bounding_geometry
{
	AABB::AABB(const std::vector<glm::vec3>& vertices, DefinitionSpace* _definitionSpace) :
		min(glm::vec3(0.0f)),
		max(glm::vec3(0.0f)),
		definitionSpace(_definitionSpace)
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

	void AABB::extendToFit(const std::vector<std::shared_ptr<BoundingGeometry>> boundingGeometries)
	{
		for (std::shared_ptr<BoundingGeometry> boundingGeometry : boundingGeometries)
		{
			for (glm::vec3& criticalPoint : boundingGeometry->getExtremaPoints())
			{
				min = glm::min(min, criticalPoint);
				max = glm::max(max, criticalPoint);
			}
		}
	}

	std::vector<glm::vec3> AABB::getExtremaPoints()
	{
		return std::vector<glm::vec3>{
			glm::vec3(min.x, min.y, min.z),
			glm::vec3(min.x, min.y, max.z),
			glm::vec3(min.x, max.y, min.z),
			glm::vec3(min.x, max.y, max.z),
			glm::vec3(max.x, min.y, min.z),
			glm::vec3(max.x, min.y, max.z),
			glm::vec3(max.x, max.y, min.z),
			glm::vec3(max.x, max.y, max.z)
		};
	}

	bool AABB::isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix)
	{
		// Translate, rotate and scale the AABB according to the model matrix.
		std::pair<glm::vec3, glm::vec3> transformedParams = definitionSpace->convertToWorldSpace(*this, modelMatrix);
		glm::vec3& transformedMin = transformedParams.first;
		glm::vec3& transformedMax = transformedParams.second;

		// AABB frustum intersection test as described in 
		// https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
		bool inFrustum = true;
		for (int i = 0; i < 6; i++) {
			glm::vec3 clippingPlane = glm::vec3(clippingPlanes[i]);
			glm::vec3 minTimesPlane = transformedMin * clippingPlane;
			glm::vec3 maxTimesPlane = transformedMax * clippingPlane;
			glm::vec3 componentwiseMax = glm::max(minTimesPlane, maxTimesPlane);

			float distanceToPlane = componentwiseMax.x + componentwiseMax.y + componentwiseMax.z + clippingPlanes[i].w;
			inFrustum &= distanceToPlane > 0;
		}
		return inFrustum;
	}

	std::pair<glm::vec3, glm::vec3> AABB::WorldSpace::convertToWorldSpace(AABB& aabb, const glm::mat4& modelMatrix)
	{
		return std::make_pair(aabb.min, aabb.max);
	}

	std::pair<glm::vec3, glm::vec3> AABB::ObjectSpace::convertToWorldSpace(AABB& aabb, const glm::mat4& modelMatrix)
	{
		glm::vec4 cornersHomogeneous[8];
		cornersHomogeneous[0] = modelMatrix * glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1);
		cornersHomogeneous[1] = modelMatrix * glm::vec4(aabb.min.x, aabb.min.y, aabb.max.z, 1);
		cornersHomogeneous[2] = modelMatrix * glm::vec4(aabb.min.x, aabb.max.y, aabb.min.z, 1);
		cornersHomogeneous[3] = modelMatrix * glm::vec4(aabb.min.x, aabb.max.y, aabb.max.z, 1);
		cornersHomogeneous[4] = modelMatrix * glm::vec4(aabb.max.x, aabb.min.y, aabb.min.z, 1);
		cornersHomogeneous[5] = modelMatrix * glm::vec4(aabb.max.x, aabb.min.y, aabb.max.z, 1);
		cornersHomogeneous[6] = modelMatrix * glm::vec4(aabb.max.x, aabb.max.y, aabb.min.z, 1);
		cornersHomogeneous[7] = modelMatrix * glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1);

		glm::vec3 corner = glm::vec3(cornersHomogeneous[0]) / cornersHomogeneous[0].w;
		glm::vec3 rotatedMin = corner;
		glm::vec3 rotatedMax = corner;
		for (int i = 1; i < 8; i++)
		{
			corner = glm::vec3(cornersHomogeneous[i]) / cornersHomogeneous[i].w;
			rotatedMin = glm::min(rotatedMin, corner);
			rotatedMax = glm::max(rotatedMax, corner);
		}

		return std::make_pair(rotatedMin, rotatedMax);
	}
}
