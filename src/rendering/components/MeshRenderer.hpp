#pragma once

#include "../bounding_geometry/BoundingGeometry.hpp"
#include "../model/Mesh.hpp"
#include "../shading/Shader.hpp"

namespace rendering::components
{
	class MeshRenderer
	{
	public:
		MeshRenderer(rendering::model::Mesh* _mesh, rendering::bounding_geometry::BoundingGeometry* _boundingGeometry) :
			mesh(_mesh),
			boundingGeometry(_boundingGeometry) {};

		void render(rendering::shading::Shader& shader, glm::mat4 modelMatrix, glm::mat4 viewProjectionMatrix);

		rendering::model::Mesh* getMesh()
		{
			return mesh;
		}

		rendering::bounding_geometry::BoundingGeometry* getBoundingGeometry()
		{
			return boundingGeometry;
		}

	private:
		rendering::model::Mesh* mesh;
		rendering::bounding_geometry::BoundingGeometry* boundingGeometry;
	};
}