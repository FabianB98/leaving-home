#include "Shader.hpp"

namespace rendering::shading
{
	LightSupportingShader::LightSupportingShader(std::string shaderName, bool useGeometryShader)
		: Shader(shaderName, useGeometryShader)
	{
		std::string maxLightsDef = definitions["MAX_LIGHT_COUNT"];
		maxPointLights = 0;
		try {
			// parse the definition to an integer
			maxPointLights = std::stoi(maxLightsDef);
			std::cout << "Setting maximum number of point lights to " + std::to_string(maxPointLights) << std::endl;
		}
		catch (const std::invalid_argument& ia) {
			std::cerr << "Can't read maximum number of points lights for shader " << shaderName << std::endl;
		}
	}

	void LightSupportingShader::setUniformDirectionalLight(const std::string name, glm::vec3 intensity, glm::vec3 direction)
	{
		setUniformVec3(name + ".intensity", intensity);
		setUniformVec3(name + ".direction", direction);
	}

	void LightSupportingShader::setUniformPointLight(const std::string name, glm::vec3 intensity, glm::vec3 position)
	{
		setUniformVec3(name + ".intensity", intensity);
		setUniformVec3(name + ".position", position);
	}

	void LightSupportingShader::setUniformPointLights(const std::string name, std::vector<glm::vec3> intensities, std::vector<glm::vec3> positions)
	{
		if (intensities.size() != positions.size())
			throw std::invalid_argument("Point light intensity and position list size must be equal!");

		unsigned int numLights = intensities.size();
		unsigned int lightsToSet = std::min(numLights, maxPointLights);

		if (numLights > maxPointLights)
			std::cout << "Warning! Only " << std::to_string(maxPointLights) << " point lights are supported. "
			<< "Lights " << std::to_string(maxPointLights) << " to " << std::to_string(numLights - 1) << " will be ignored." << std::endl;

		for (unsigned int i = 0; i < lightsToSet; ++i)
			setUniformPointLight(name + "[" + std::to_string(i) + "]", intensities[i], positions[i]);

		// fill up the unused point light uniforms with zeros
		if (numLights >= maxPointLights) return;
		for (unsigned int i = numLights; i < maxPointLights; ++i)
			setUniformPointLight(name + "[" + std::to_string(i) + "]", glm::vec3(0), glm::vec3(0));

	}
}