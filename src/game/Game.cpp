#include "Game.hpp"

#include "../rendering/model/Mesh.hpp"
#include "../rendering/shading/Shader.hpp"
#include "../rendering/shading/Lights.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glfw/glfw3.h>


namespace game
{
	rendering::model::Mesh* mesh;
	rendering::Shader* shader;

	void Game::init(rendering::RenderingEngine* renderingEngine)
	{
		shader = new rendering::Shader("phong");
		mesh = new rendering::model::Mesh("test");

		renderingEngine->setClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));
	}

	void Game::input(double deltaTime)
	{

	}

	void Game::update(double deltaTime)
	{

	}

	void Game::render(rendering::RenderingEngine* renderingEngine)
	{
		// TEMPORARY TEST CODE
		int width = renderingEngine->getFramebufferWidth();
		int height = renderingEngine->getFrameBufferHeight();

		glm::vec3 camPos(0, 0, 5);
		glm::mat4 projection = glm::perspective(glm::radians(45.f), (float) width/ (float) height, .1f, 1000.f);
		glm::mat4 view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 model = glm::rotate((float) glfwGetTime(), glm::vec3(0,1,0)) * glm::scale(glm::vec3(1.5));
		glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model)));

		glm::mat4 mvp = projection * view * model;

		shader->use();

		shader->setUniformMat4("T_MVP", mvp);
		shader->setUniformMat4("T_M", model);
		shader->setUniformMat3("T_Normal", normal);
		shader->setUniformVec3("cameraPos", camPos);

		shader->setUniformVec3("kA", glm::vec3(0.1));
		shader->setUniformVec3("kD", glm::vec3(0.3));
		shader->setUniformVec3("kS", glm::vec3(1, 1, 1));
		shader->setUniformInt("n", 8);

		rendering::DirectionalLight light;
		light.intensity = glm::vec3(1, 1, 1);
		light.direction = glm::vec3(2, 1, 1);
		shader->setUniformDirectionalLight("directionalLight", light);

		rendering::PointLight pLight;
		pLight.intensity = glm::vec3(0, 0, 1);
		pLight.position = glm::vec3(-1.5, -1.5, 1.5);
		shader->setUniformPointLight("pointLights[0]", pLight);

		mesh->render();
	}

	void Game::cleanUp()
	{
		delete mesh;
	}
}
