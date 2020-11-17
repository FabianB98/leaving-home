#include "Shader.hpp"

namespace rendering
{
	Shader::Shader(std::string shaderName)
	{
		// Create the shaders
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);


		std::string vertexShaderCode = loadShaderFile(shaderName + ".vert");
		std::string fragmentShaderCode = loadShaderFile(shaderName + ".frag");
		
		loadDefinitions(vertexShaderCode);
		loadDefinitions(fragmentShaderCode);



		glDetachShader(programID, vertexShader);
		glDetachShader(programID, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	// UNIFORM FUNCTIONS - wrappers for the glUniform functions to access uniforms by their names

	void Shader::setUniformInt(const std::string name, const int value)
	{
		GLuint id = getUniformLocation(name);
		glUniform1i(id, value);
	}

	void Shader::setUniformVec3(const std::string name, const glm::vec3& vector)
	{
		GLuint id = getUniformLocation(name);
		glUniform3f(id, vector.x, vector.y, vector.z);
	}

	void Shader::setUniformMat3(const std::string name, const glm::mat3& matrix)
	{
		GLuint id = getUniformLocation(name);
		glUniformMatrix3fv(id, 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::setUniformMat4(const std::string name, const glm::mat4& matrix)
	{
		GLuint id = getUniformLocation(name);
		glUniformMatrix4fv(id, 1, GL_FALSE, &matrix[0][0]);
	}


	// UNIFORM LIGHT FUNCTIONS - directional light and point lights

	void Shader::setUniformDirectionalLight(const std::string name, const DirectionalLight& light)
	{
		setUniformVec3(name + ".intensity", light.intensity);
		setUniformVec3(name + ".direction", light.direction);
	}

	void Shader::setUniformPointLight(const std::string name, const PointLight& light)
	{
		setUniformVec3(name + ".intensity", light.intensity);
		setUniformVec3(name + ".position", light.position);
	}

	void Shader::setUniformPointLights(const std::string name, const std::vector<PointLight> lights)
	{
		//TODO: check vector size < max point lights
		for (unsigned int i = 0; i < lights.size(); ++i)
			setUniformPointLight(name + "[" + std::to_string(i) + "]", lights[i]);
	}

	GLuint Shader::getUniformLocation(std::string name)
	{
		// returns the location from the cache or gets it from OpenGL
		auto it = uniforms.find(name);
		if (it == uniforms.end())
			return uniforms[name] = glGetUniformLocation(programID, name.c_str());

		return it->second;
	}


	std::string Shader::loadShaderFile(std::string fileName)
	{
		// Read the Vertex Shader code from the file
		std::string shaderCode;
		std::ifstream fileStream("res/shaders/" + fileName, std::ios::in);
		if (!fileStream.is_open()) {
			printf("Can't open shader file %s!\n", fileName);
			return shaderCode;
		}

		std::stringstream stream;
		stream << fileStream.rdbuf();
		shaderCode = stream.str();
		fileStream.close();

		return shaderCode;
	}

	void Shader::loadDefinitions(std::string shaderCode)
	{
		
	}
}