#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory>
#include <functional>

// OpenGL related headers
#include <GL/glew.h>
#include <glm/glm.hpp>

// Our headers
#include "MeshPart.hpp"
#include "../bounding_geometry/BoundingGeometry.hpp"
#include "../bounding_geometry/None.hpp"
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
			) : Mesh(vertices, uvs, normals, _parts, std::make_shared<bounding_geometry::None>()) {}

			Mesh(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::vector<std::shared_ptr<MeshPart>>& _parts,
				std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry
			);

			Mesh(std::string assetName) : Mesh(assetName, std::make_shared<bounding_geometry::None>()) {}

			Mesh(std::string assetName, std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry);

			~Mesh();

			template <typename DataType>
			void addAdditionalVertexAttributeF(GLuint location, std::vector<DataType> data, GLint size, GLenum type)
			{
				addAdditionalVertexAttributeF(location, data, size, type, GL_FALSE, 0, (void*)0);
			}

			template <typename DataType>
			void addAdditionalVertexAttributeF(
				GLuint location,
				std::vector<DataType> data,
				GLint size,
				GLenum type,
				GLboolean normalized,
				GLsizei stride,
				const void* pointer
			) {
				addAdditionalVertexAttribute(location, data, [location, size, type, normalized, stride, pointer]() {
					glVertexAttribPointer(location, size, type, normalized, stride, pointer);
				});
			}

			template <typename DataType>
			void addAdditionalVertexAttributeI(GLuint location, std::vector<DataType> data, GLint size, GLenum type)
			{
				addAdditionalVertexAttributeI(location, data, size, type, 0, (void*)0);
			}

			template <typename DataType>
			void addAdditionalVertexAttributeI(
				GLuint location,
				std::vector<DataType> data,
				GLint size,
				GLenum type,
				GLsizei stride,
				const void* pointer
			) {
				addAdditionalVertexAttribute(location, data, [location, size, type, stride, pointer]() {
					glVertexAttribIPointer(location, size, type, stride, pointer);
				});
			}

			template <typename DataType>
			void addAdditionalVertexAttributeL(GLuint location, std::vector<DataType> data, GLint size, GLenum type)
			{
				addAdditionalVertexAttributeL(location, data, size, type, 0, (void*)0);
			}

			template <typename DataType>
			void addAdditionalVertexAttributeL(
				GLuint location,
				std::vector<DataType> data,
				GLint size,
				GLenum type,
				GLsizei stride,
				const void* pointer
			) {
				addAdditionalVertexAttribute(location, data, [location, size, type, stride, pointer]() {
					glVertexAttribLPointer(location, size, type, stride, pointer);
				});
			}

			void render(shading::Shader& shader);

			void renderInstanced(
				shading::Shader& shader,
				const std::vector<glm::mat4>& modelMatricesIndexed,
				const std::vector<glm::mat3>& normalMatricesIndexed,
				const std::vector<glm::mat4>& mvpMatrices,
				const std::vector<std::size_t>& instanceIndices
			);


			const std::shared_ptr<bounding_geometry::BoundingGeometry> getBoundingGeometry()
			{
				return boundingGeometry;
			}

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

			std::vector<GLuint> additionalVbos;
			std::unordered_set<GLuint> usedAttributeLocations;

			std::vector<std::shared_ptr<MeshPart>> parts;

			std::shared_ptr<bounding_geometry::BoundingGeometry> boundingGeometry;

			size_t maxInstancesDrawn;

			void initOpenGlBuffers(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals
			);

			template <typename DataType>
			void addAdditionalVertexAttribute(
				GLuint location,
				std::vector<DataType> data,
				std::function<void()> const& setVertexAttribPointer
			) {
				// Ensure that the given location is not already used by some other vertex attribute.
				if (usedAttributeLocations.find(location) != usedAttributeLocations.end())
					throw std::invalid_argument("Location already in use!");
				usedAttributeLocations.insert(location);

				// Bind the VAO as we're about to add a new VBO to it.
				glBindVertexArray(vao);

				// Create a VBO, fill it with the given data and bind it to the given vertex attribute location.
				GLuint vbo;
				glGenBuffers(1, &vbo);
				additionalVbos.push_back(vbo);

				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(DataType), &data[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(location);
				setVertexAttribPointer();

				// Unbind the VAO to ensure that it won't be changed by any other piece of code by accident.
				glBindVertexArray(0);
			}
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
