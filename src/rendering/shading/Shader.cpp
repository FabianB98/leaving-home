#include "Shader.hpp"

namespace rendering::shading
{
	Shader::Shader(std::string shaderName, bool useGeometryShader)
	{
		std::cout << "Loading shader " << shaderName << std::endl;

		// Create the shaders
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint geometryShader;
		if (useGeometryShader) geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		// Load shader code
		std::cout << " Loading shader files" << std::endl;
		auto vertexShaderCode = loadShaderFile(shaderName + ".vert");
		auto fragmentShaderCode = loadShaderFile(shaderName + ".frag");
		std::string geometryShaderCode;
		if (useGeometryShader) geometryShaderCode = loadShaderFile(shaderName + ".geom");

		loadDefinitions(vertexShaderCode);
		loadDefinitions(fragmentShaderCode);
		loadDefinitions(geometryShaderCode); // this also works with an empty string

		// Compile and link shaders
		std::cout << " Compiling shader files" << std::endl;
		compileShader(vertexShader, vertexShaderCode);
		compileShader(fragmentShader, fragmentShaderCode);
		if (useGeometryShader) compileShader(geometryShader, geometryShaderCode);

		std::cout << " Linking shaders" << std::endl;
		if (useGeometryShader) linkProgram({ vertexShader, fragmentShader, geometryShader });
		else linkProgram({ vertexShader, fragmentShader });


		// Clean up
		cleanUpShader(vertexShader);
		cleanUpShader(fragmentShader);
		if (useGeometryShader) cleanUpShader(geometryShader);

		pass = RenderPass::FORWARD;
		auto def = definitions.find("render");
		if (def != definitions.end())
			pass = renderPassValueOf(def->second);
	}

	void Shader::use()
	{
		glUseProgram(programID);
	}

	// UNIFORM FUNCTIONS - wrappers for the glUniform functions to access uniforms by their names

	void Shader::setUniformInt(const std::string name, const GLint value)
	{
		GLuint id = getUniformLocation(name);
		glUniform1i(id, value);
	}

	void Shader::setUniformFloat(const std::string name, const float value)
	{
		GLuint id = getUniformLocation(name);
		glUniform1f(id, value);
	}

	void Shader::setUniformVec2(const std::string name, const glm::vec2& vector)
	{
		GLuint id = getUniformLocation(name);
		glUniform2f(id, vector.x, vector.y);
	}

	void Shader::setUniformVec3(const std::string name, const glm::vec3& vector)
	{
		GLuint id = getUniformLocation(name);
		glUniform3f(id, vector.x, vector.y, vector.z);
	}

	void Shader::setUniformVec3List(const std::string name, const std::vector<glm::vec3>& vectors)
	{
		for (unsigned int i = 0; i < vectors.size(); ++i)
			setUniformVec3(name + "[" + std::to_string(i) + "]", vectors[i]);
	}

	void Shader::setUniformVec4(const std::string name, const glm::vec4& vector)
	{
		GLuint id = getUniformLocation(name);
		glUniform4f(id, vector.x, vector.y, vector.z, vector.w);
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


	GLuint Shader::getUniformLocation(std::string name)
	{
		// returns the location from the cache or gets it from OpenGL
		auto it = uniforms.find(name);
		if (it == uniforms.end())
			return uniforms[name] = glGetUniformLocation(programID, name.c_str());

		return it->second;
	}

	void Shader::loadDefinitions(std::string shaderCode)
	{
		constexpr auto definition = "#define";
		constexpr auto spaceChars = " ";

		if (shaderCode.empty()) return;

		std::stringstream input(shaderCode);
		std::string line;

		while (std::getline(input, line, '\n'))
		{
			// check if line is a definition
			unsigned int start = line.find_first_not_of(spaceChars);
			if (line.find(definition) != start) continue;

			// split line into elements separated by spaces
			std::vector<std::string> elements;
			size_t offset = line.find_first_not_of(spaceChars, start + std::strlen(definition));

			while (offset != std::string::npos)
			{
				// find end of element and add substring to vector
				size_t end = line.find_first_of(spaceChars, offset);
				std::string element = line.substr(offset, end - offset);
				elements.push_back(element);

				// find start of new element
				offset = line.find_first_not_of(spaceChars, end);
			}

			std::string name = elements[0];
			std::string value = elements[1];

			// a definition with this name already exists
			if (definitions.find(name) != definitions.end()) continue;
			definitions[name] = value;
		}
	}

	void Shader::linkProgram(std::initializer_list<GLuint> shaders)
	{
		// Link the program
		programID = glCreateProgram();
		for (auto shader : shaders)
			glAttachShader(programID, shader);
		glLinkProgram(programID);

		// check linked program
		GLint result = GL_FALSE;
		int logLength;
		glGetProgramiv(programID, GL_COMPILE_STATUS, &result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			std::vector<char> errorMessage(logLength + 1);
			glGetProgramInfoLog(programID, logLength, NULL, &errorMessage[0]);
			std::cerr << &errorMessage[0] << std::endl;
			throw std::invalid_argument("Shaders could not be linked! See output for more details.");
		}
	}

	std::string Shader::loadShaderFile(std::string fileName)
	{
		// Read the Vertex Shader code from the file
		std::string shaderCode;
		std::ifstream fileStream("res/shaders/" + fileName, std::ios::in);
		if (!fileStream.is_open()) {
			throw std::invalid_argument("Can't open shader file " + fileName + "!");
			return shaderCode;
		}

		std::stringstream stream;
		stream << fileStream.rdbuf();
		shaderCode = stream.str();
		fileStream.close();

		return shaderCode;
	}

	void Shader::compileShader(GLuint shader, std::string code)
	{
		GLint result = GL_FALSE;
		int logLength;

		const char* codePtr = code.c_str();
		glShaderSource(shader, 1, &codePtr, NULL);
		glCompileShader(shader);

		// check compiled shader
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			std::vector<char> errorMessage(logLength + 1);
			glGetShaderInfoLog(shader, logLength, NULL, &errorMessage[0]);
			std::cerr << &errorMessage[0] << std::endl;
			throw std::invalid_argument("Shader could not be compiled! See output for more details.");
		}
	}

	void Shader::cleanUpShader(GLuint shader)
	{
		glDetachShader(programID, shader);
		glDeleteShader(shader);
	}
}