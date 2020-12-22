#pragma once

#include <array>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace rendering::bounding_geometry 
{
	class BoundingGeometry
	{
	public:
		virtual void fitToVertices(const std::vector<glm::vec3>& vertices) = 0;

		virtual void extendToFit(const std::vector<std::shared_ptr<BoundingGeometry>> boundingGeometries) = 0;

		virtual std::vector<glm::vec3> getExtremaPoints() = 0;

		virtual bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix) = 0;
	};
}
