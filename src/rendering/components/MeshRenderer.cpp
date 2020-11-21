#include "MeshRenderer.hpp"

namespace rendering::components
{
	void MeshRenderer::render(rendering::shading::Shader& shader, glm::mat4 modelMatrix, glm::mat4 viewProjectionMatrix)
	{
		glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
		glm::mat4 mvp = viewProjectionMatrix * modelMatrix;

		shader.setUniformMat4("T_MVP", mvp);
		shader.setUniformMat4("T_M", modelMatrix);
		shader.setUniformMat3("T_Normal", normal);

		mesh->render(shader);
	}
}