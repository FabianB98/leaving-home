#include "Material.hpp"

namespace rendering
{
	namespace model
	{
		void Material::bind(shading::Shader& shader)
		{
			shader.setUniformVec3("kA", ambient);
			shader.setUniformVec3("kD", diffuse);
			shader.setUniformVec3("kS", specular);
			shader.setUniformInt("n", phongExponent);
		}

		bool Material::operator==(const Material other) const
		{
			return ambient == other.ambient
				&& diffuse == other.diffuse
				&& specular == other.specular
				&& phongExponent == other.phongExponent;
		}
	}
}
