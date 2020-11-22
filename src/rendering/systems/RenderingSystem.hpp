#pragma once

#include <entt/entt.hpp>

#include "../components/Camera.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/Transform.hpp"
#include "../components/Lights.hpp"
#include "../shading/Shader.hpp"

namespace rendering::systems
{
	void updateLights(entt::registry& registry, rendering::shading::LightSupportingShader& shader);

	void renderRenderingSystem(entt::registry& registry,
		rendering::components::Camera& camera, rendering::shading::Shader& shader);
}
