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
		struct MeshPartData
		{
			std::shared_ptr<Material> material;
			std::vector<unsigned int> indices;
			GLenum mode;

			MeshPartData() : material(nullptr), indices(std::vector<unsigned int>()), mode(GL_TRIANGLES) {}

			MeshPartData(const std::shared_ptr<Material>& _material, const std::vector<unsigned int>& _indices)
				: material(_material), indices(_indices), mode(GL_TRIANGLES) {}

			MeshPartData(const std::shared_ptr<Material>& _material, const std::vector<unsigned int>& _indices, GLenum _mode)
				: material(_material), indices(_indices), mode(_mode) {}
		};

		class MeshPart
		{
		public:
			MeshPart(std::shared_ptr<Material> _material, const std::vector<unsigned int>& indices)
				: MeshPart(_material, indices, GL_TRIANGLES) {}

			MeshPart(std::shared_ptr<Material> _material, const std::vector<unsigned int>& indices, GLenum _mode);

			MeshPart(const MeshPartData& data) : MeshPart(data.material, data.indices, data.mode) {}

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
