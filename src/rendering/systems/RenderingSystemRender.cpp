#include "RenderingSystem.hpp"

namespace rendering::systems
{
	extern ShaderManager* shaderManager;

	extern std::unordered_map<model::Mesh*, std::pair<std::vector<glm::mat4>, std::vector<glm::mat3>>> meshTransforms;
	extern std::vector<std::pair<model::Mesh*, shading::Shader*>> meshShaders;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat4>> modelMatricesToRender;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat3>> normalMatricesToRender;
	extern std::unordered_map<model::Mesh*, std::vector<glm::mat4>> mvpMatricesToRender;

	extern glm::mat4 viewMatrix;
	extern glm::mat3 viewNormalMatrix;

	void activateShader(entt::registry& registry, rendering::components::Camera& camera, shading::Shader& shader)
	{
		shader.use();
		camera.applyViewProjection(shader);
		updateDirectionalLights(registry, shader);

		if (shader.getRenderPass() == shading::RenderPass::GEOMETRY) {
			shader.setUniformMat4("T_V", viewMatrix);
			shader.setUniformMat3("T_V_Normal", viewNormalMatrix);
		}

		shaderManager->setUniforms(shader);
	}

	void renderPicking(entt::registry& registry, rendering::components::Camera& camera, shading::Shader* pickingShader)
	{
		auto& picking = registry.ctx<Picking>();
		if (picking.enabled.size() == 0) return;

		pickingShader->use();
		camera.applyViewProjection(*pickingShader);

		for (const auto& mesh : picking.enabled)
		{
			const auto& meshInstances = meshTransforms[mesh];
			const auto& meshModels = modelMatricesToRender[mesh];
			size_t numInstances = meshModels.size();

			if (numInstances > 0)
			{
				const auto& meshNormals = normalMatricesToRender[mesh];
				const auto& meshMVPs = mvpMatricesToRender[mesh];

				// Render all instances of the mesh.
				mesh->renderInstanced(*pickingShader, meshModels, meshNormals, meshMVPs);
			}
		}
	}

	void renderMeshes(entt::registry& registry, rendering::components::Camera& camera, shading::RenderPass filter)
	{
		// Render each mesh with the corresponding shader.
		shading::Shader* activeShader = NULL;
		for (const auto& meshShaderPairs : meshShaders)
		{
			auto* mesh = meshShaderPairs.first;
			auto* shader = meshShaderPairs.second;
			if (shader->getRenderPass() != filter) continue;

			const auto& meshModels = modelMatricesToRender[mesh];
			size_t numInstances = meshModels.size();

			if (numInstances > 0)
			{
				// switch shader if necessary
				if (shader != activeShader || activeShader == NULL) {
					activeShader = shader;
					activateShader(registry, camera, *activeShader);
				}

				const auto& meshNormals = normalMatricesToRender[mesh];
				const auto& meshMVPs = mvpMatricesToRender[mesh];

				// Render all instances of the mesh.
				mesh->renderInstanced(*activeShader, meshModels, meshNormals, meshMVPs);
			}
		}
	}

	void renderForward(entt::registry& registry, rendering::components::Camera& camera)
	{
		renderMeshes(registry, camera, shading::RenderPass::FORWARD);
	}


	void renderDeferredGPass(entt::registry& registry, rendering::components::Camera& camera)
	{
		renderMeshes(registry, camera, shading::RenderPass::GEOMETRY);
	}


	void renderDeferredLightingPass(entt::registry& registry, rendering::components::Camera& camera)
	{
		renderMeshes(registry, camera, shading::RenderPass::LIGHTING);
	}
}