#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "../../rendering/RenderingEngine.hpp"
#include "../../rendering/components/Transform.hpp"
#include "../components/FirstPersonRotateController.hpp"
#include "../components/FreeFlyingMoveController.hpp"

#include <glfw/glfw3.h>

namespace game::systems
{
	void updateMovementInputSystem(rendering::RenderingEngine* renderingEngine, double deltaTime);
}
