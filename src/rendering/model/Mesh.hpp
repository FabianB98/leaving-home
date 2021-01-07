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
		enum class VertexAttributeType
		{
			SINGLE_PRECISION, DOUBLE_PRECISION, INTEGER
		};

		struct IVertexAttribute
		{
		public:
			GLint size;
			VertexAttributeType attributeType;
			GLenum dataType;
			GLboolean normalized{ GL_FALSE };
			GLsizei stride{ 0 };
			void* pointer{ (void*)0 };

			IVertexAttribute(
				GLint _size,
				VertexAttributeType _attributeType,
				GLenum _dataType,
				GLboolean _normalized,
				GLsizei _stride,
				void* _pointer
			) : size(_size),
				attributeType(_attributeType),
				dataType(_dataType),
				normalized(_normalized),
				stride(_stride),
				pointer(_pointer) {}

			IVertexAttribute(GLint _size, VertexAttributeType _attributeType, GLenum _dataType)
				: size(_size), attributeType(_attributeType), dataType(_dataType) {}

			virtual void* getData() = 0;

			virtual GLint getDataSize() = 0;

		private:
			virtual std::shared_ptr<IVertexAttribute> createNewVertexAttributeOfThisType() = 0;

			virtual void addData(const std::shared_ptr<IVertexAttribute>& attribute) = 0;

			friend struct MeshData;
		};

		template <typename DataType>
		struct VertexAttribute : public IVertexAttribute
		{
		public:
			std::vector<DataType> attributeData;

			VertexAttribute(
				GLint _size,
				VertexAttributeType _attributeType,
				GLenum _dataType,
				GLboolean _normalized,
				GLsizei _stride,
				void* _pointer
			) : IVertexAttribute(_size, _attributeType, _dataType, _normalized, _stride, _pointer) {}

			VertexAttribute(GLint _size, VertexAttributeType _attributeType, GLenum _dataType)
				: IVertexAttribute(_size, _attributeType, _dataType) {}

			void* getData()
			{
				return &attributeData[0];
			}

			GLint getDataSize() 
			{
				return attributeData.size() * sizeof(DataType);
			}

		private:
			std::shared_ptr<IVertexAttribute> createNewVertexAttributeOfThisType()
			{
				return std::make_shared<VertexAttribute<DataType>>(size, attributeType, dataType, normalized, stride, pointer);
			}

			void addData(const std::shared_ptr<IVertexAttribute>& attribute)
			{
				auto attributeCasted = std::dynamic_pointer_cast<VertexAttribute<DataType>>(attribute);

				for (DataType& data : attributeCasted->attributeData)
					attributeData.push_back(data);
			}
		};

		struct MeshDataInstance
		{
			glm::mat4 transformation;
			std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>> additionalVertexAttributes;

			MeshDataInstance(glm::mat4 _transformation) : transformation(_transformation) {}

			MeshDataInstance(
				glm::mat4 _transformation,
				const std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>>& _additionalVertexAttributes
			) : transformation(_transformation), additionalVertexAttributes(_additionalVertexAttributes) {}
		};

		struct MeshData
		{
		public:
			std::vector<glm::vec3> vertices;
			std::vector<glm::vec2> uvs;
			std::vector<glm::vec3> normals;
			std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>> additionalVertexAttributes;
			std::vector<std::shared_ptr<MeshPartData>> parts;

			MeshData() {}

			MeshData(
				const std::vector<glm::vec3>& _vertices,
				const std::vector<glm::vec2>& _uvs,
				const std::vector<glm::vec3>& _normals,
				const std::vector<std::shared_ptr<MeshPartData>>& _parts
			) : MeshData(_vertices, _uvs, _normals, std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>>(), _parts) {}

			MeshData(
				const std::vector<glm::vec3>& _vertices,
				const std::vector<glm::vec2>& _uvs,
				const std::vector<glm::vec3>& _normals,
				const std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>> _additionalVertexAttributes,
				const std::vector<std::shared_ptr<MeshPartData>>& _parts
			) : vertices(_vertices),
				uvs(_uvs),
				normals(_normals),
				additionalVertexAttributes(_additionalVertexAttributes),
				parts(_parts) {}

			MeshData(const std::vector<std::pair<std::shared_ptr<MeshData>, std::vector<MeshDataInstance>>> instances);

			MeshData(std::string assetName);

		private:
			void addAdditionalVertexAttributeData(GLuint location, std::shared_ptr<IVertexAttribute> attributeData);
		};

		class Mesh
		{
		public:
			Mesh(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::vector<std::shared_ptr<MeshPart>>& _parts
			) : Mesh(
					vertices,
					uvs,
					normals,
					_parts,
					std::make_shared<bounding_geometry::None>()
				) {}

			Mesh(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>> additionalVertexAttributes,
				const std::vector<std::shared_ptr<MeshPart>>& _parts
			) : Mesh(vertices, uvs, normals, additionalVertexAttributes, _parts, std::make_shared<bounding_geometry::None>()) {}

			Mesh(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::vector<std::shared_ptr<MeshPart>>& _parts,
				std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry
			) : Mesh(
					vertices,
					uvs,
					normals,
					std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>>(),
					_parts,
					_boundingGeometry
				) {}

			Mesh(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>> additionalVertexAttributes,
				const std::vector<std::shared_ptr<MeshPart>>& _parts,
				std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry
			);

			Mesh(const MeshData& data) : Mesh(data, std::make_shared<bounding_geometry::None>()) {}

			Mesh(const MeshData& data, std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry)
				: Mesh(
					data.vertices,
					data.uvs,
					data.normals,
					data.additionalVertexAttributes,
					createMeshParts(data.parts),
					_boundingGeometry
				) {}

			Mesh(const std::vector<std::pair<std::shared_ptr<MeshData>, std::vector<MeshDataInstance>>> instances)
				: Mesh(instances, std::make_shared<bounding_geometry::None>()) {}

			Mesh(
				const std::vector<std::pair<std::shared_ptr<MeshData>, std::vector<MeshDataInstance>>> instances,
				std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry
			) : Mesh(MeshData(instances), _boundingGeometry) {}

			Mesh(std::string assetName) : Mesh(assetName, std::make_shared<bounding_geometry::None>()) {}

			Mesh(std::string assetName, std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry)
				: Mesh(MeshData(assetName), _boundingGeometry) {}

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

			void setData(const MeshData& data);

			void render(shading::Shader& shader);

			void renderInstanced(
				shading::Shader& shader,
				const std::vector<glm::mat4>& modelMatrices,
				const std::vector<glm::mat3>& normalMatrices,
				const std::vector<glm::mat4>& mvpMatrices
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

			std::unordered_map<GLuint, GLuint> additionalVbos;
			std::unordered_set<GLuint> usedAttributeLocations;

			std::vector<std::shared_ptr<MeshPart>> parts;

			std::shared_ptr<bounding_geometry::BoundingGeometry> boundingGeometry;

			size_t maxInstancesDrawn;

			static std::vector<std::shared_ptr<MeshPart>> createMeshParts(const std::vector<std::shared_ptr<MeshPartData>>& parts);

			void initOpenGlBuffers(
				const std::vector<glm::vec3>& vertices,
				const std::vector<glm::vec2>& uvs,
				const std::vector<glm::vec3>& normals,
				const std::unordered_map<GLuint, std::shared_ptr<IVertexAttribute>> additionalVertexAttributes
			);

			template <typename DataType>
			void addAdditionalVertexAttribute(
				GLuint location,
				std::vector<DataType> data,
				std::function<void()> const& setVertexAttribPointer
			) {
				addAdditionalVertexAttribute(location, &data[0], data.size() * sizeof(DataType), setVertexAttribPointer);
			}

			void addAdditionalVertexAttribute(
				GLuint location,
				void* data,
				GLint size,
				std::function<void()> const& setVertexAttribPointer
			);

			void setAdditionalVertexAttributeData(
				GLuint location,
				void* data,
				GLint size,
				std::function<void()> const& setVertexAttribPointer
			);

			void setAdditionalVertexAttributeData(GLuint location, std::shared_ptr<IVertexAttribute> data);
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
