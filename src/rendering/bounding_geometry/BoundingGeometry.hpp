#pragma once

#include <array>

#include <glm/glm.hpp>

namespace rendering::bounding_geometry 
{
	class BoundingGeometry
	{
	public:
		virtual bool isInCameraFrustum(std::array<glm::vec4, 6> clippingPlanes, glm::mat4 modelMatrix) = 0;
	};
}
