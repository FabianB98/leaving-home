#pragma once

#include <algorithm>
#include <execution>
#include <set>
#include <vector>
#include <unordered_map>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../components/Camera.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/Transform.hpp"
#include "../components/Relationship.hpp"
#include "../components/Lights.hpp"
#include "../shading/Shader.hpp"

namespace rendering::systems
{
	void initRenderingSystem(entt::registry& registry);

	void updateLights(entt::registry& registry, rendering::shading::LightSupportingShader& shader);

	void renderRenderingSystem(entt::registry& registry,
		rendering::components::Camera& camera, rendering::shading::Shader& shader);

	void cleanUpRenderingSystem(entt::registry& registry);
}
