#pragma once

#include <algorithm>
#include <execution>
#include <queue>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../components/Camera.hpp"
#include "../components/CullingGeometry.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/Transform.hpp"
#include "../components/Relationship.hpp"
#include "../components/Lights.hpp"
#include "../shading/Shader.hpp"

namespace rendering::systems
{
	void initRenderingSystem(entt::registry& registry);

	void cullingRelationship(entt::registry& registry, entt::entity parent, entt::entity child);

	void updateDirectionalLights(entt::registry& registry, rendering::shading::Shader& shader);

	void renderUpdateTransforms(entt::registry& registry,
		rendering::components::Camera& camera, rendering::shading::Shader* defaultShader, bool overrideShaders = false);


	void activateShader(entt::registry& registry, rendering::components::Camera& camera, shading::Shader& shader, uint32_t pickingID);


	void renderForward(entt::registry& registry, rendering::components::Camera& camera, uint32_t pickingID);


	void renderDeferredGPass(entt::registry& registry, rendering::components::Camera& camera, uint32_t pickingID);


	void renderDeferredLightingPass(entt::registry& registry, rendering::components::Camera& camera);



	void renderPicking(entt::registry& registry,
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
