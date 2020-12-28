#include "Shader.hpp"

namespace rendering::shading
{
	LightSupportingShader::LightSupportingShader(std::string shaderName, bool useGeometryShader)
		: Shader(shaderName, useGeometryShader)
	{
		std::string maxLightsDef = definitions["MAX_LIGHT_COUNT"];
		maxDirectionalLights = 0;
		try {
			// parse the definition to an integer
			maxDirectionalLights = std::stoi(maxLightsDef);
			std::cout << "Setting maximum number of directional lights to " + std::to_string(maxDirectionalLights) << std::endl;
		}
		catch (const std::invalid_argument& ia) {
			std::cerr << "Can't read maximum number of directional lights for shader " << shaderName << std::endl;
		}
	}

	void LightSupportingShader::setUniformDirectionalLight(const std::string name, glm::vec3 intensity, glm::vec3 direction)
	{
		setUniformVec3(name + ".intensity", intensity);
		setUniformVec3(name + ".direction", direction);
	}

	void LightSupportingShader::setUniformDirectionalLights(const std::string name, std::vector<glm::vec3> intensities, std::vector<glm::vec3> directions)
	{
		if (intensities.size() != directions.size())
			throw std::invalid_argument("Directional light intensity and direction list size must be equal!");

		unsigned int numLights = intensities.size();
		unsigned int lightsToSet = std::min(numLights, maxDirectionalLights);
		if (lightsToSet == 0) return;

		if (numLights > maxDirectionalLights)
			std::cout << "Warning! Only " << std::to_string(maxDirectionalLights) << " directional lights are supported. "
			<< "Lights " << std::to_string(maxDirectionalLights) << " to " << std::to_string(numLights - 1) << " will be ignored." << std::endl;

		for (unsigned int i = 0; i < lightsToSet; ++i)
			setUniformDirectionalLight(name + "[" + std::to_string(i) + "]", intensities[i], directions[i]);

		// fill up the unused directional light uniforms with zeros
		if (numLights >= maxDirectionalLights) return;
		for (unsigned int i = numLights; i < maxDirectionalLights; ++i)
			setUniformDirectionalLight(name + "[" + std::to_string(i) + "]", glm::vec3(0), glm::vec3(0));

	}
}