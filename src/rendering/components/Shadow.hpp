#pragma once

#include <entt/entt.hpp>

namespace rendering::components
{
	struct CastShadow
	{
		CastShadow(entt::entity _directionalLight) : directionalLight(_directionalLight) {}

		entt::entity directionalLight;
	};
}