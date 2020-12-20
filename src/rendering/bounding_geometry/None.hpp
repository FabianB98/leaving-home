#pragma once

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class None : public BoundingGeometry
	{
	public:
		bool isInCameraFrustum(std::array<glm::vec4, 6> clippingPlanes, glm::mat4 modelMatrix)
		{
			return true;
		}
	};
}