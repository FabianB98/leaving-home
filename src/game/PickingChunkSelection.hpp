#pragma once

#include <entt/entt.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "../rendering/RenderingEngine.hpp"
#include "../rendering/components/Camera.hpp"
#include "world/World.hpp"

namespace game
{
	void selectChunks(entt::registry& registry, rendering::RenderingEngine* renderingEngine, world::World* wrld);
}