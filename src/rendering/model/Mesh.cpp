#include "Mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../lib/tinyobjloader/tiny_obj_loader.h"

namespace rendering
{
	namespace model
	{
		MeshData::MeshData(std::string assetName)
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

			// tinyobjloader returns a data structure where the positions, UV coordinates and normals have different 
			// indices. As OpenGL can only handle index buffers with shared indices (i.e. positions, UV coordinates and
			// normals must use the same index when calling glDrawElements), we need to create combined indices (and 
			// therefore duplicate some of the positions, UV coordinates and normals). The following quite ugly piece of
			// code will perform this task.

			// A map for storing the individual index combinations we found so far.
			std::map<std::tuple<int, int, int>, unsigned int> combinedIndexMap;
			unsigned int nextId = 0;

			// The indices for each material.
			std::map<int, std::vector<unsigned int>*> indicesMap;

			// Iterate over all shapes (i.e. all objects stored in the loaded obj file).
			for (size_t shapeIndex = 0; shapeIndex < shapes.size(); ++shapeIndex)
			{
				tinyobj::shape_t& shape = shapes[shapeIndex];

				// Iterate over all faces of the current shape.
				for (size_t faceIndex = 0; faceIndex < shape.mesh.num_face_vertices.size(); ++faceIndex)
				{
					// Extract the material ID of the current face and get the indices for that material.
					int materialId = shape.mesh.material_ids[faceIndex];
					auto materialEntry = indicesMap.find(materialId);
					std::vector<unsigned int>* materialIndices;
					if (materialEntry != indicesMap.end())
					{
						// We have already seen a face for this material. Append any following indices to the mesh part
						// of this material.
						materialIndices = materialEntry->second;
					}
					else
					{
						// We have not yet seen a face for this material. Crate a new list of indices for the mesh part
						// of this material.
						materialIndices = new std::vector<unsigned int>();
						indicesMap[materialId] = materialIndices;
					}

					// Iterate over all vertices of the current face. As tinyobjloader has triangulated the mesh for us,
					// we can safely assume that all faces have exactly three vertices.
					for (size_t i = 0; i < 3; ++i)
					{
						// Extract the vertex index, the UV index and the normal index from the current vertex.
						tinyobj::index_t& index = shape.mesh.indices[3 * faceIndex + i];
						int& vertexIndex = index.vertex_index;
						int& uvIndex = index.texcoord_index;
						int& normalIndex = index.normal_index;

						// Create an identifier for the combined vertex index and check whether we have already seen
						// this combined vertex index.
						std::tuple<int, int, int> combinedIndexId =
							std::tuple<int, int, int>(vertexIndex, uvIndex, normalIndex);
						auto entry = combinedIndexMap.find(combinedIndexId);
						unsigned int combinedIndex;
						if (entry != combinedIndexMap.end())
						{
							// We have already seen this combination of vertex position index, UV index and normal
							// index. Reuse the data from the first occurrence of this combination.
							combinedIndex = entry->second;
						}
						else
						{
							// We have not yet seen this combination of vertex position index, UV index and normal
							// index. Create a new index for this combination and mark this combination as seen.
							combinedIndex = nextId;
							combinedIndexMap[combinedIndexId] = combinedIndex;
							nextId++;

							// As this combination was not seen yet and a new index was created, we also need to add
							// the actual data to the data buffers.
							vertices.push_back(glm::vec3(
								attrib.vertices[3 * (size_t)vertexIndex],
								attrib.vertices[3 * (size_t)vertexIndex + 1],
								attrib.vertices[3 * (size_t)vertexIndex + 2]
							));
							uvs.push_back(glm::vec2(
								attrib.texcoords[2 * (size_t)uvIndex],
								attrib.texcoords[2 * (size_t)uvIndex + 1]
							));
							normals.push_back(glm::vec3(
								attrib.normals[3 * (size_t)normalIndex],
								attrib.normals[3 * (size_t)normalIndex + 1],
								attrib.normals[3 * (size_t)normalIndex + 2]
							));
						}

						// Add the combined index to the indices of the current material.
						materialIndices->push_back(combinedIndex);
					}
				}
			}

			// Create a MeshPartData for each material.
			for (auto const& materialIndices : indicesMap)
			{
				// Create a Material instance from the values loaded by the tinyobjloader.
				tinyobj::material_t& mat = materials[materialIndices.first];
				std::shared_ptr<Material> material = std::make_shared<Material>(
					glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]),
					glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]),
					glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]),
					mat.shininess
				);

				// Add a new MeshPartData instance with the material and the indices to the parts of this MeshData.
				parts.push_back(MeshPartData(material, *materialIndices.second));
				delete materialIndices.second;
			}
		}

		Mesh::Mesh(
			const std::vector<glm::vec3>& vertices,
			const std::vector<glm::vec2>& uvs,
			const std::vector<glm::vec3>& normals,
			const std::vector<std::shared_ptr<MeshPart>>& _parts,
			std::shared_ptr<bounding_geometry::BoundingGeometry> _boundingGeometry
		) : parts(_parts), boundingGeometry(_boundingGeometry), maxInstancesDrawn(0)
		{
			initOpenGlBuffers(vertices, uvs, normals);
			boundingGeometry->fitToVertices(vertices);
		}

		std::vector<std::shared_ptr<MeshPart>> Mesh::createMeshParts(const std::vector<MeshPartData>& parts)
		{
			std::vector<std::shared_ptr<MeshPart>> result;

			for (const MeshPartData& part : parts)
				result.push_back(std::make_shared<MeshPart>(part.material, part.indices, part.mode));

			return result;
		}

		void Mesh::initOpenGlBuffers(
			const std::vector<glm::vec3>& vertices,
			const std::vector<glm::vec2>& uvs,
			const std::vector<glm::vec3>& normals
		)
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

			// Create 4 VBOs for the model matrices and bind it to vertex attributes 3, 4, 5 and 6. These VBOs are only
			// used when using instanced rendering.
			glGenBuffers(1, &modelMatrixVbo);
			glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVbo);
			for (int i = 0; i < 4; i++)
			{
				glEnableVertexAttribArray(3 + i);
				glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(i * sizeof(glm::vec4)));
				glVertexAttribDivisor(3 + i, 1);
			}

			// Create 3 VBOs for the normal matrices and bind it to vertex attribute 7, 8 and 9. These VBOs are only
			// used when using instanced rendering.
			glGenBuffers(1, &normalMatrixVbo);
			glBindBuffer(GL_ARRAY_BUFFER, normalMatrixVbo);
			for (int i = 0; i < 3; i++)
			{
				glEnableVertexAttribArray(7 + i);
				glVertexAttribPointer(7 + i, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(i * sizeof(glm::vec3)));
				glVertexAttribDivisor(7 + i, 1);
			}

			// Create 4 VBOs for the model-view-projection matrices and bind it to vertex attribute 10, 11, 12 and 13.
			// These VBOs are only used when using instanced rendering.
			glGenBuffers(1, &mvpMatrixVbo);
			glBindBuffer(GL_ARRAY_BUFFER, mvpMatrixVbo);
			for (int i = 0; i < 4; i++)
			{
				glEnableVertexAttribArray(10 + i);
				glVertexAttribPointer(10 + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(i * sizeof(glm::vec4)));
				glVertexAttribDivisor(10 + i, 1);
			}

			// Unbind the VAO to ensure that it won't be changed by any other piece of code by accident.
			glBindVertexArray(0);

			// Store that the attribute locations 0 through 13 are in use. Therefore no other vertex buffer can be added
			// for these locations.
			for (int i = 0; i < 14; i++)
				usedAttributeLocations.insert(i);
		}

		Mesh::~Mesh()
		{
			glDeleteVertexArrays(1, &vao);

			glDeleteBuffers(1, &vertexVbo);
			glDeleteBuffers(1, &uvVbo);
			glDeleteBuffers(1, &normalVbo);

			glDeleteBuffers(1, &modelMatrixVbo);
			glDeleteBuffers(1, &normalMatrixVbo);
			glDeleteBuffers(1, &mvpMatrixVbo);

			for (auto& vbo : additionalVbos)
				glDeleteBuffers(1, &vbo);
		}

		void Mesh::render(rendering::shading::Shader& shader)
		{
			glBindVertexArray(vao);
			for (auto part : parts)
				part->render(shader);
			glBindVertexArray(0);
		}

		void Mesh::renderInstanced(
			shading::Shader& shader,
			const std::vector<glm::mat4>& modelMatrices,
			const std::vector<glm::mat3>& normalMatrices,
			const std::vector<glm::mat4>& mvpMatrices
		) {
			size_t numInstances = modelMatrices.size();

			glBindVertexArray(vao);

			if (numInstances > maxInstancesDrawn)
			{
				maxInstancesDrawn = numInstances;

				glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVbo);
				glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), &modelMatrices[0], GL_STREAM_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, normalMatrixVbo);
				glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat3), &normalMatrices[0], GL_STREAM_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, mvpMatrixVbo);
				glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), &mvpMatrices[0], GL_STREAM_DRAW);
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, numInstances * sizeof(glm::mat4), &modelMatrices[0]);

				glBindBuffer(GL_ARRAY_BUFFER, normalMatrixVbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, numInstances * sizeof(glm::mat3), &normalMatrices[0]);

				glBindBuffer(GL_ARRAY_BUFFER, mvpMatrixVbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, numInstances * sizeof(glm::mat4), &mvpMatrices[0]);
			}

			for (auto part : parts)
				part->renderInstanced(shader, numInstances);

			glBindVertexArray(0);
		}
	}
}
