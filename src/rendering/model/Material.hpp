#pragma once

#include <glm/glm.hpp>

#include "../shading/Shader.hpp"

namespace rendering
{
	namespace model
	{
		class Material
		{
		private:
			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
			float phongExponent;

		public:
			Material(glm::vec3& _ambient, glm::vec3& _diffuse, glm::vec3& _specular, float _phongExponent) :
				ambient(_ambient), diffuse(_diffuse), specular(_specular), phongExponent(_phongExponent) {};

			void bind(Shader& shader);
		};
	}
}
