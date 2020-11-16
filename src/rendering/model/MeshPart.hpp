#pragma once

// Standard headers
#include <stdlib.h>
#include <vector>

// OpenGL related headers
#include <GL/glew.h>

// Our headers
#include "Material.hpp"

namespace rendering
{
	namespace model
	{
		class MeshPart
		{
		public:
			MeshPart(Material* _material, const std::vector<unsigned int>& indices);

			~MeshPart();

			void render();

		private:
			Material* material;

			GLuint indexBuffer;
			GLsizei numIndices;
		};
	}
}
