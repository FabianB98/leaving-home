#pragma once

#include <entt/entt.hpp>

namespace rendering::components
{
	struct CastShadow
	{
		CastShadow(entt::entity _directionalLight) : directionalLight(_directionalLight), shadowMult(1.f) {}

		entt::entity directionalLight;
		float shadowMult;
	};
}