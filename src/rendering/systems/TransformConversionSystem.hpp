#pragma once

#include <entt/entt.hpp>

#include "../components/Transform.hpp"

namespace rendering::systems
{
	void updateTransformConversion(entt::registry& registry);
}
