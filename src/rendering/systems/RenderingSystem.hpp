#pragma once

#include <algorithm>
#include <execution>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../components/Camera.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/Transform.hpp"
#include "../components/Relationship.hpp"
#include "../components/Lights.hpp"
#include "../shading/Shader.hpp"

namespace rendering::systems
{
	void initRenderingSystem(entt::registry& registry);

	void updateLights(entt::registry& registry, rendering::shading::Shader& shader);

	void renderRenderingSystemTransforms(entt::registry& registry,
		rendering::components::Camera& camera, rendering::shading::Shader* defaultShader, bool overrideShaders = false);

	void renderRenderingSystemForward(entt::registry& registry, rendering::components::Camera& camera, uint32_t pickingID);

	void renderRenderingSystemPicking(entt::registry& registry,
		rendering::components::Camera& camera, shading::Shader* pickingShader);

	void cleanUpRenderingSystem(entt::registry& registry);

	struct MeshShading
	{
		std::unordered_map<model::Mesh*, shading::Shader*> shaders;
		std::unordered_map<shading::Shader*, int> priorities;
	};

	struct Picking
	{
		std::unordered_set<model::Mesh*> enabled;
	};
}
