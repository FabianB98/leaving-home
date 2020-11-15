#include "Mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../lib/tinyobjloader/tiny_obj_loader.h"

namespace rendering
{
	namespace model
	{
		Mesh::Mesh(
			std::vector<glm::vec3>& vertices,
			std::vector<glm::vec2>& uvs,
			std::vector<glm::vec3>& normals,
			std::vector<MeshPart>& _parts
		) : parts(_parts)
		{
			// Create a Vertex Array Object (VAO).
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// Create a Vertex Buffer (VBO), fill it with the meshs's vertices and bind it to vertex attribute 0.
			glGenBuffers(1, &vertexVbo);
			glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			// Create a VBO, fill it with the mesh's UV coordinates and bind it to vertex attribute 1.
			glGenBuffers(1, &uvVbo);
			glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
			glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			// Create a VBO, fill it with the mesh's normals and bind it to vertex attribute 2.
			glGenBuffers(1, &normalVbo);
			glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			// Unbind the VAO to ensure that it won't be changed by any other piece of code by accident.
			glBindVertexArray(0);
		}

		Mesh::Mesh(std::string assetName)
		{
			// Construct the actual path to the obj file.
			std::string fileName = "./res/models/" + assetName + ".obj";

			// The results of the LoadObj function.
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warnings;
			std::string errors;

			// Try to load the obj file.
			bool success = tinyobj::LoadObj(
				&attrib, &shapes, &materials,
				&warnings, &errors,
				fileName.c_str(), "./res/models/");

			// Print any errors and warnings that might have occurred while trying to load the obj file.
			if (!errors.empty())
				std::cerr << "An error occurred while trying to load mesh " << assetName << "!" << std::endl
				<< errors << std::endl;
			if (!warnings.empty())
				std::cout << "A warning occurred while trying to load mesh " << assetName << "!" << std::endl
				<< warnings << std::endl;

			// Assert that loading the obj file was successful.
			if (!success)
				throw std::runtime_error(errors);

			// TODO: Extract vertices, uvs, normals and parts from the results and call the other constructor with these.
			for (int shapeIndex = 0; shapeIndex < shapes.size(); ++shapeIndex)
			{
				tinyobj::shape_t& shape = shapes[shapeIndex];
				std::cout << "Shape " << shapeIndex << ": " << shape.name << std::endl;

				for (int faceIndex = 0; faceIndex < shape.mesh.num_face_vertices.size(); ++faceIndex)
				{
					int faceVertices = shape.mesh.num_face_vertices[faceIndex];
					int materialId = shape.mesh.material_ids[faceIndex];

					for (int i = 0; i < faceVertices; ++i)
					{
						int vertexIndex = 3 * faceIndex + i;

						tinyobj::index_t& index = shape.mesh.indices[vertexIndex];
						std::cout << vertexIndex << " " << faceIndex << " " << materialId << " " << index.vertex_index << " " << index.texcoord_index << " " << index.normal_index << std::endl;
					}
				}
			}
		}

		Mesh::~Mesh()
		{
			glDeleteVertexArrays(1, &vao);

			glDeleteBuffers(1, &vertexVbo);
			glDeleteBuffers(1, &uvVbo);
			glDeleteBuffers(1, &normalVbo);
		}

		void Mesh::render()
		{
			glBindVertexArray(vao);
			for (auto part : parts)
				part.render();
			glBindVertexArray(0);
		}
	}
}
