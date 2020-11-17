#pragma once

// Standard headers
#include <stdlib.h>
#include <vector>
#include <memory>

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
			MeshPart(std::shared_ptr<Material> _material, const std::vector<unsigned int>& indices);

			~MeshPart();

			void render();

		private:
			std::shared_ptr<Material> material;

			GLuint indexBuffer;
			GLsizei numIndices;
		};
	}
}