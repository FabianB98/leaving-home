#pragma once

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class None : public BoundingGeometry
	{
	public:
		void fitToVertices(const std::vector<glm::vec3>& vertices)
		{
			// Nothing to do here...
		}

		virtual void extendToFit(const std::vector<std::shared_ptr<BoundingGeometry>> boundingGeometries)
		{
			// Nothing to do here...
		}

		virtual std::vector<glm::vec3> getExtremaPoints()
		{
			// This operation doesn't make any sense for bounding geometries implying that the object should always be
			// rendered. We're making the assumption here that a None bounding geometry will never be used to extend
			// another bounding geometry.
			return std::vector<glm::vec3>();
		}

		bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix)
		{
			return true;
		}
	};
}