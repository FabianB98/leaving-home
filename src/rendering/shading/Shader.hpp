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
#include <stdexcept>
#include <initializer_list>

// OpenGL related headers
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace rendering::shading
{
	class Shader 
	{
	public:
		Shader(std::string shaderName, bool useGeometryShader = false);

		void use();

		void setUniformInt(const std::string name, const GLint value);
		void setUniformFloat(const std::string name, const float value);
		void setUniformVec2(const std::string name, const glm::vec2& vector);
		void setUniformVec3(const std::string name, const glm::vec3& vector);
		void setUniformVec4(const std::string name, const glm::vec4& vector);
		void setUniformMat3(const std::string name, const glm::mat3& matrix);
		void setUniformMat4(const std::string name, const glm::mat4& matrix);

		virtual void setUniformDirectionalLight(const std::string name, glm::vec3 intensity, glm::vec3 direction) {}
		virtual void setUniformDirectionalLights(const std::string name, std::vector<glm::vec3> intensities, std::vector<glm::vec3> directions) {}
		//virtual void setUniformPointLight(const std::string name, glm::vec3 intensity, glm::vec3 position) {}
		//virtual void setUniformPointLights(const std::string name, std::vector<glm::vec3> intensities, std::vector<glm::vec3> positions) {}

		bool usesDeferredRendering()
		{
			return deferred;
		}
		
	protected:
		GLuint programID;
		// map for average constant time lookup of cached uniform locations
		std::unordered_map<std::string, GLuint> uniforms;
		// map for #define values in the shader code
		std::unordered_map<std::string, std::string> definitions;

		GLuint getUniformLocation(std::string name);

	private:
		bool deferred;

		void loadDefinitions(std::string shaderCode);
		void linkProgram(std::initializer_list<GLuint> shaders);
		void cleanUpShader(GLuint shader);
		
		static std::string loadShaderFile(std::string fileName);
		static void compileShader(GLuint shader, std::string code);
	};

	class LightSupportingShader : public Shader
	{
	public:
		LightSupportingShader(std::string shaderName, bool useGeometryShader = false);

		void setUniformDirectionalLight(const std::string name, glm::vec3 intensity, glm::vec3 direction);
		void setUniformDirectionalLights(const std::string name, std::vector<glm::vec3> intensities, std::vector<glm::vec3> directions);
		/*void setUniformPointLight(const std::string name, glm::vec3 intensity, glm::vec3 position);
		void setUniformPointLights(const std::string name, std::vector<glm::vec3> intensities, std::vector<glm::vec3> positions);*/

	private:
		unsigned int maxDirectionalLights{ 0 };
	};
}
