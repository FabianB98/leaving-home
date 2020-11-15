#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

// OpenGL related headers
#include <GL/glew.h>
#include <glm/glm.hpp>

// Our headers
#include "MeshPart.hpp"

namespace rendering
{
	namespace model
	{
		class Mesh
		{
		public:
			Mesh(
				std::vector<glm::vec3>& vertices,
				std::vector<glm::vec2>& uvs,
				std::vector<glm::vec3>& normals,
				std::vector<MeshPart>& _parts
			);

			Mesh(std::string assetName);

			~Mesh();

			void render();

		private:
			GLuint vao;

			GLuint vertexVbo;
			GLuint uvVbo;
			GLuint normalVbo;

			std::vector<MeshPart> parts;
		};
	}
}
