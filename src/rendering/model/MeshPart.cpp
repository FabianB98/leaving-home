#include "MeshPart.hpp"

namespace rendering
{
	namespace model
	{
		MeshPart::MeshPart(std::shared_ptr<Material> _material, const std::vector<unsigned int>& indices, GLenum _mode)
			: material(std::move(_material)), numIndices((GLsizei)indices.size()), mode(_mode)
		{
			glGenBuffers(1, &indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		MeshPart::~MeshPart()
		{
			glDeleteBuffers(1, &indexBuffer);
		}

		void MeshPart::setData(std::shared_ptr<MeshPartData> data)
		{
			material = data->material;
			numIndices = data->indices.size();
			mode = data->mode;

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indices.size() * sizeof(unsigned int), &data->indices[0], GL_STATIC_DRAW);
		}

		void MeshPart::render(rendering::shading::Shader& shader)
		{
			material->bind(shader);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glDrawElements(mode, numIndices, GL_UNSIGNED_INT, (void*)0);
		}

		void MeshPart::renderInstanced(shading::Shader& shader, size_t numInstances)
		{
			material->bind(shader);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glDrawElementsInstanced(mode, numIndices, GL_UNSIGNED_INT, (void*)0, numInstances);
		}
	}
}