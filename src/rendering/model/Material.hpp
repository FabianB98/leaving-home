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

			void bind(shading::Shader& shader);

			bool operator==(const Material other) const;

			friend struct std::hash<Material>;
		};
	}
}

namespace std {
	template <>
	struct hash<rendering::model::Material>
	{
		std::size_t operator()(const rendering::model::Material& material) const
		{
			size_t hashValue = 17;

			hashValue = hashValue * 31 + hash<float>()(material.ambient.r);
			hashValue = hashValue * 31 + hash<float>()(material.ambient.g);
			hashValue = hashValue * 31 + hash<float>()(material.ambient.b);

			hashValue = hashValue * 31 + hash<float>()(material.diffuse.r);
			hashValue = hashValue * 31 + hash<float>()(material.diffuse.g);
			hashValue = hashValue * 31 + hash<float>()(material.diffuse.b);

			hashValue = hashValue * 31 + hash<float>()(material.specular.r);
			hashValue = hashValue * 31 + hash<float>()(material.specular.g);
			hashValue = hashValue * 31 + hash<float>()(material.specular.b);
			
			hashValue = hashValue * 31 + hash<float>()(material.phongExponent);

			return hashValue;
		}
	};
}
