#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

// OpenGL related headers
#include <GL/glew.h>
#include <glm/glm.hpp>

// Our headers
#include "MeshPart.hpp"
#include "../shading/Shader.hpp"

namespace rendering
{
	namespace model
	{
		class Mesh
		{
		public:
			Mesh(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::vector<std::shared_ptr<MeshPart>>& _parts
			);

			Mesh(std::string assetName);

			~Mesh();

			void render(shading::Shader& shader);

			void renderInstanced(
				shading::Shader& shader, 
				const std::vector<glm::mat4>& modelMatrices, 
				const std::vector<glm::mat3>& normalMatrices, 
				const std::vector<glm::mat4>& mvpMatrices
			);

			bool operator==(const Mesh& other) const
			{
				return vao == other.vao;
			}

			size_t const hashValue() const
			{
				return std::hash<GLuint>{}(vao);
			}

		private:
			GLuint vao;

			GLuint vertexVbo;
			GLuint uvVbo;
			GLuint normalVbo;

			GLuint modelMatrixVbo;
			GLuint normalMatrixVbo;
			GLuint mvpMatrixVbo;

			std::vector<std::shared_ptr<MeshPart>> parts;

			void initOpenGlBuffers(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals
			);
		};
	}
}

namespace std {
	template<> struct hash<rendering::model::Mesh>
	{
		size_t operator()(rendering::model::Mesh const& mesh) const
		{
			return mesh.hashValue();
		}
	};
}
