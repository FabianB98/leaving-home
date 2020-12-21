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

		bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix)
		{
			return true;
		}
	};
}