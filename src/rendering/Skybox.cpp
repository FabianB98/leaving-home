#include "Skybox.hpp"

namespace rendering
{
	Skybox::Skybox(std::string cubemapName, std::string shaderName)
	{
		cube = std::make_shared<model::Mesh>("unitcube");
		cubemap = std::make_shared<textures::Cubemap>(cubemapName);
		shader = std::make_shared<shading::Shader>(shaderName);
	}

	void Skybox::use()
	{
		shader->use();
	}

	void Skybox::render(RenderingEngine* engine, GLuint defaultDepthFunc)
	{
		auto& registry = engine->getRegistry();
		auto camera = engine->getMainCamera();

		auto cameraComponent = registry.get<components::Camera>(camera);
		
		auto camView = cameraComponent.getViewMatrix();
		auto projection = cameraComponent.getProjectionMatrix();

		glDepthFunc(GL_LEQUAL);

		glm::mat4 mvp = projection * glm::mat4(glm::mat3(camView));

		shader->setUniformMat4("T_MVP", mvp);
		cubemap->bind(*shader, "cubemap");
		cube->render(*shader);

		glDepthFunc(defaultDepthFunc);
	}
}