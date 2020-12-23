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

		void extendToFitGeometry(const std::shared_ptr<BoundingGeometry> boundingGeometry)
		{
			extendToFitGeometries(std::vector<std::shared_ptr<BoundingGeometry>>{ boundingGeometry });
		}

		virtual void extendToFitGeometries(const std::vector<std::shared_ptr<BoundingGeometry>> boundingGeometries) = 0;

		virtual std::vector<glm::vec3> getExtremaPoints() = 0;

		virtual std::shared_ptr<BoundingGeometry> toWorldSpace(const glm::mat4& modelMatrix) = 0;

		virtual bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix) = 0;
	};
}
