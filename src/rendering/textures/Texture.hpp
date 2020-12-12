#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

// OpenGL related headers
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

// Headers related to stb.
#include <stb_image.h>

// our headers
#include "../shading/Shader.hpp"

namespace rendering::textures
{
	class Cubemap
	{
	public:
		Cubemap(std::string name);

		void bind(rendering::shading::Shader& shader, std::string uniformName, unsigned int index = 0);

	private:
		static const std::vector<std::string> const faces;

		GLuint id;
	};

	class Texture2D
	{
	public:
		Texture2D(std::string name);

		void bind(rendering::shading::Shader& shader, std::string uniformName, unsigned int index = 0);

		GLuint getId() { return id; }
	private:
		GLuint id;
	};
}