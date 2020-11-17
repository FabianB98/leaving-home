#pragma once

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// OpenGL related headers
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Lights.hpp"

namespace rendering 
{
	class Shader 
	{
	public:
		Shader(std::string shaderName);

		void use() { glUseProgram(programID); }

		void setUniformInt(const std::string name, const int value);
		void setUniformVec3(const std::string name, const glm::vec3& vector);
		void setUniformMat3(const std::string name, const glm::mat3& matrix);
		void setUniformMat4(const std::string name, const glm::mat4& matrix);

		void setUniformDirectionalLight(const std::string name, const DirectionalLight& light);
		void setUniformPointLight(const std::string name, const PointLight& light);
		void setUniformPointLights(const std::string name, const std::vector<PointLight> lights);
		
	private:
		GLuint programID;
		// map for average constant time lookup of cached uniform locations
		std::unordered_map<std::string, GLuint> uniforms;
		// map for #define values in the shader code
		std::unordered_map<std::string, std::string> definitions;

		void loadDefinitions(std::string shaderCode);
		GLuint getUniformLocation(std::string name);
		
		static std::string loadShaderFile(std::string fileName);
	};
}
