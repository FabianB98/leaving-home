#pragma once

#include <unordered_set>

#include <entt/entt.hpp>

#include "../bounding_geometry/BoundingGeometry.hpp"
#include "../bounding_geometry/None.hpp"

namespace rendering::components
{
	struct CullingGeometry
	{
		CullingGeometry(std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry) : boundingGeometry(_boundingGeometry) {}

		std::shared_ptr<bounding_geometry::BoundingGeometry> boundingGeometry;

		entt::entity parent{ entt::null };
		std::unordered_set<entt::entity> children;
	};
}
