#include "Material.hpp"

namespace rendering
{
	namespace model
	{
		void Material::bind(Shader& shader)
		{
			shader.setUniformVec3("kA", ambient);
			shader.setUniformVec3("kD", diffuse);
			shader.setUniformVec3("kS", specular);
			shader.setUniformInt("n", phongExponent);
		}
	}
}
