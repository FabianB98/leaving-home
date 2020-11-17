#include "MeshPart.hpp"

namespace rendering
{
	namespace model
	{
		MeshPart::MeshPart(std::shared_ptr<Material> _material, const std::vector<unsigned int>& indices)
			: material(std::move(_material)), numIndices((GLsizei)indices.size())
		{
			glGenBuffers(1, &indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		MeshPart::~MeshPart()
		{
			glDeleteBuffers(1, &indexBuffer);
		}

		void MeshPart::render()
		{
			material->bind();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (void*)0);
		}
	}
}