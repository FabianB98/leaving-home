#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

// OpenGL related headers
#include <GL/glew.h>
#include <glm/glm.hpp>

// Our headers
#include "MeshPart.hpp"
#include "../shading/Shader.hpp"

namespace rendering
{
	namespace model
	{
		class Mesh
		{
		public:
			Mesh(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::vector<std::shared_ptr<MeshPart>>& _parts
			);

			Mesh(std::string assetName);

			~Mesh();

			void render(Shader& shader);

		private:
			GLuint vao;

			GLuint vertexVbo;
			GLuint uvVbo;
			GLuint normalVbo;

			std::vector<std::shared_ptr<MeshPart>> parts;

			void initOpenGlBuffers(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals
			);
		};
	}
}
