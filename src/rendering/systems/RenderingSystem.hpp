#pragma once

#include <entt/entt.hpp>

#include "../components/Camera.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/Transform.hpp"
#include "../shading/Shader.hpp"

namespace rendering::systems
{
	void renderRenderingSystem(entt::registry& registry,
		rendering::components::Camera& camera, rendering::shading::Shader& shader);
}
