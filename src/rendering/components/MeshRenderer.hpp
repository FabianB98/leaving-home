#pragma once

#include "../bounding_geometry/BoundingGeometry.hpp"
#include "../model/Mesh.hpp"
#include "../shading/Shader.hpp"

namespace rendering::components
{
	class MeshRenderer
	{
	public:
		MeshRenderer(rendering::model::Mesh* _mesh) : mesh(_mesh) {};

		void render(rendering::shading::Shader& shader, glm::mat4 modelMatrix, glm::mat4 viewProjectionMatrix);

		rendering::model::Mesh* getMesh()
		{
			return mesh;
		}

	private:
		rendering::model::Mesh* mesh;
	};
}