#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"

namespace rendering::textures
{
	const std::vector<std::string> const Cubemap::faces
	{
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"
	};

	Cubemap::Cubemap(std::string name)
	{
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, channels;
		for (unsigned int i = 0; i < faces.size(); i++) {
			auto fileName = "res/textures/" + name + "/" + faces[i] + ".png";
			unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &channels, 0);
			
			if (!data) {
				stbi_image_free(data);
				throw std::invalid_argument("Can't read texture " + fileName + "!");
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		id = textureID;
	}

	void Cubemap::bind(rendering::shading::Shader& shader, std::string uniformName, unsigned int index)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		shader.setUniformInt(uniformName, id);
	}
}