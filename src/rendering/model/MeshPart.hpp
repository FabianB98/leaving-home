#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.hpp"

namespace rendering
{
	namespace model
	{
		class MeshPart
		{
		public:
			MeshPart(Material& _material, std::vector<unsigned int>& indices);

			~MeshPart();

			void render();

		private:
			Material material;
			GLuint indexBuffer;
		};
	}
}
