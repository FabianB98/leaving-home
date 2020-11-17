#include "Shader.hpp"

namespace rendering
{
	Shader::Shader(std::string shaderName)
	{
		// Create the shaders
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Load shader code
		std::cout << "Loading shader files" << std::endl;
		std::string vertexShaderCode = loadShaderFile(shaderName + ".vert");
		std::string fragmentShaderCode = loadShaderFile(shaderName + ".frag");
		loadDefinitions(vertexShaderCode);
		loadDefinitions(fragmentShaderCode);

		// Compile and link shaders
		std::cout << "Compiling shader files" << std::endl;
		compileShader(vertexShader, vertexShaderCode);
		compileShader(fragmentShader, fragmentShaderCode);

		std::cout << "Linking shaders" << std::endl;
		linkProgram(vertexShader, fragmentShader);


		// Clean up
		glDetachShader(programID, vertexShader);
		glDetachShader(programID, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Shader::use()
	{
		glUseProgram(programID);
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

	void Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader)
	{
		// Link the program
		programID = glCreateProgram();
		glAttachShader(programID, vertexShader);
		glAttachShader(programID, fragmentShader);
		glLinkProgram(programID);

		// check linked program
		GLint result = GL_FALSE;
		int logLength;
		glGetProgramiv(programID, GL_COMPILE_STATUS, &result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			std::vector<char> errorMessage(logLength + 1);
			glGetProgramInfoLog(programID, logLength, NULL, &errorMessage[0]);
			//printf("%s\n", &errorMessage[0]);
			std::cerr << &errorMessage[0] << std::endl;
		}
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
			//printf("%s\n", &errorMessage[0]);
			std::cerr << &errorMessage[0] << std::endl;
		}
	}
}