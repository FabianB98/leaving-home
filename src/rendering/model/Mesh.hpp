#pragma once

#include <stdlib.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "MeshPart.hpp"

namespace rendering
{
	namespace model
	{
		class Mesh
		{
		public:
			Mesh(std::vector<vec3>& vertices, std::vector<vec2>& uvs, std::vector<vec3>& normals, std::vector<MeshPart>& _parts);

			Mesh(const char* assetName);

			~Mesh();

			void render();

		private:
			GLuint vertexBuffer;
			GLuint uvBuffer;
			GLuint normalBuffer;

			std::vector<MeshPart> parts;
		};
	}
}
