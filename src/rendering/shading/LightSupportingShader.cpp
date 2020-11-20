#include "Shader.hpp"

namespace rendering
{
	LightSupportingShader::LightSupportingShader(std::string shaderName) : Shader(shaderName)
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

	void LightSupportingShader::setUniformDirectionalLight(const std::string name, const DirectionalLight& light)
	{
		setUniformVec3(name + ".intensity", light.getIntensity());
		setUniformVec3(name + ".direction", light.getDirection());
	}

	void LightSupportingShader::setUniformPointLight(const std::string name, const PointLight& light)
	{
		setUniformVec3(name + ".intensity", light.getIntensity());
		setUniformVec3(name + ".position", light.getPosition());
	}

	void LightSupportingShader::setUniformPointLights(const std::string name, const std::vector<PointLight> lights)
	{
		unsigned int numLights = lights.size();
		unsigned int lightsToSet = std::min(numLights, maxPointLights);

		if (numLights > maxPointLights)
			std::cout << "Warning! Only " << std::to_string(maxPointLights) << " point lights are supported. "
			<< "Lights " << std::to_string(maxPointLights) << " to " << std::to_string(numLights - 1) << " will be ignored." << std::endl;

		for (unsigned int i = 0; i < lightsToSet; ++i)
			setUniformPointLight(name + "[" + std::to_string(i) + "]", lights[i]);

		// fill up the unused point light uniforms with zeros
		if (numLights >= maxPointLights) return;
		for (unsigned int i = numLights; i < maxPointLights; ++i)
			setUniformPointLight(name + "[" + std::to_string(i) + "]", _UNUSED);

	}
}