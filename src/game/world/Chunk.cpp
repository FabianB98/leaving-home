#include "Chunk.hpp"

#include <stdio.h>

namespace game::world
{
	static const glm::vec2 UP = glm::vec2(0, CELL_SIZE);
	static const glm::vec2 DIAG_RIGHT_UP = glm::vec2(CELL_SIZE * cos(glm::radians(30.0f)), CELL_SIZE * sin(glm::radians(30.0f)));
	static const glm::vec2 DIAG_RIGHT_DOWN = glm::vec2(CELL_SIZE * cos(glm::radians(330.0f)), CELL_SIZE * sin(glm::radians(330.0f)));
	static const glm::vec2 CENTER_LINE_START = - glm::vec2(CELL_SIZE, CELL_SIZE) * DIAG_RIGHT_UP;

	rendering::model::Mesh* Chunk::generateMesh()
	{
		// Generate the positions of all points used as starting positions for possible cells within the current chunk.
		std::vector<glm::vec2> positions;

		unsigned int sum = 0;
		unsigned int lineIndexPrefixsum[2 * CHUNK_SIZE + 1];

		for (int line = -CHUNK_SIZE; line <= CHUNK_SIZE; line++)
		{
			glm::vec2 lineStart = CENTER_LINE_START;
			if (line < 0)
				lineStart -= (float)line * UP;
			else
				lineStart += (float)line * DIAG_RIGHT_DOWN;

			int pointsInLine = 2 * CHUNK_SIZE + 1 - abs(line);
			for (int pointInLine = 0; pointInLine < pointsInLine; pointInLine++)
			{
				positions.push_back(lineStart + (float)pointInLine * DIAG_RIGHT_UP);
			}

			lineIndexPrefixsum[line + CHUNK_SIZE] = sum;
			sum += pointsInLine;
		}

		// Generate a mesh from the generated positions.
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		for (auto& position : positions)
		{
			vertices.push_back(glm::vec3(position.x, 0, position.y));
			uvs.push_back(glm::vec2(0, 0));
			normals.push_back(glm::vec3(0, 1, 0));
		}

		std::vector<unsigned int> indices;

		// Upper-left half of the hexagon
		for (int line = 1; line <= CHUNK_SIZE; line++)
		{
			// Generate right-pointing triangle.
			indices.push_back(lineIndexPrefixsum[line] + 1);
			indices.push_back(lineIndexPrefixsum[line]);
			indices.push_back(lineIndexPrefixsum[line - 1]);

			for (int pointInLine = 1; pointInLine < CHUNK_SIZE + line; pointInLine++)
			{
				// Generate left-pointing triangle.
				indices.push_back(lineIndexPrefixsum[line] + pointInLine);
				indices.push_back(lineIndexPrefixsum[line - 1] + pointInLine - 1);
				indices.push_back(lineIndexPrefixsum[line - 1] + pointInLine);

				// Generate right-pointing triangle.
				indices.push_back(lineIndexPrefixsum[line] + pointInLine + 1);
				indices.push_back(lineIndexPrefixsum[line] + pointInLine);
				indices.push_back(lineIndexPrefixsum[line - 1] + pointInLine);
			}
		}

		// Bottom-right half of the hexagon
		for (int line = CHUNK_SIZE + 1; line <= 2 * CHUNK_SIZE; line++)
		{
			int pointsInLine = 3 * CHUNK_SIZE - line;
			for (int pointInLine = 0; pointInLine < pointsInLine; pointInLine++)
			{
				// Generate left-pointing triangle.
				indices.push_back(lineIndexPrefixsum[line] + pointInLine);
				indices.push_back(lineIndexPrefixsum[line - 1] + pointInLine);
				indices.push_back(lineIndexPrefixsum[line - 1] + pointInLine + 1);

				// Generate right-pointing triangle.
				indices.push_back(lineIndexPrefixsum[line] + pointInLine + 1);
				indices.push_back(lineIndexPrefixsum[line] + pointInLine);
				indices.push_back(lineIndexPrefixsum[line - 1] + pointInLine + 1);
			}

			// Generate left-pointing triangle.
			indices.push_back(lineIndexPrefixsum[line] + pointsInLine);
			indices.push_back(lineIndexPrefixsum[line - 1] + pointsInLine);
			indices.push_back(lineIndexPrefixsum[line - 1] + pointsInLine + 1);
		}

		std::shared_ptr<rendering::model::Material> material = std::make_shared<rendering::model::Material>(
			glm::vec3(0.0f, 0.2f, 0.0f), 
			glm::vec3(0.0f, 1.0f, 0.0f), 
			glm::vec3(0.0f, 0.1f, 0.0f), 
			2.0f
		);
		std::vector<std::shared_ptr<rendering::model::MeshPart>> meshParts;
		meshParts.push_back(std::make_shared<rendering::model::MeshPart>(material, indices));
		return new rendering::model::Mesh(vertices, uvs, normals, meshParts);
	}
}
