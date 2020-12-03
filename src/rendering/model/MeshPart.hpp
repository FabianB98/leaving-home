#pragma once

// Standard headers
#include <stdlib.h>
#include <vector>
#include <memory>

// OpenGL related headers
#include <GL/glew.h>

// Our headers
#include "Material.hpp"
#include "../shading/Shader.hpp"

namespace rendering
{
	namespace model
	{
		class MeshPart
		{
		public:
			MeshPart(std::shared_ptr<Material> _material, const std::vector<unsigned int>& indices)
				: MeshPart(_material, indices, GL_TRIANGLES) {};

			MeshPart(std::shared_ptr<Material> _material, const std::vector<unsigned int>& indices, GLenum _mode);

			~MeshPart();

			void render(shading::Shader& shader);

			void renderInstanced(shading::Shader& shader, size_t numInstances);

		private:
			std::shared_ptr<Material> material;

			GLuint indexBuffer;
			GLsizei numIndices;

			GLenum mode;
		};
	}
}
