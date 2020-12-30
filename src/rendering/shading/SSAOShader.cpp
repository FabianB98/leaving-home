#include "Shader.hpp"

namespace rendering::shading
{
	SSAOShader::SSAOShader(std::string shaderName, bool useGeometryShader)
		: Shader(shaderName, useGeometryShader)
	{
		std::string noiseSizeString = definitions["NOISE_SIZE"];
		std::string kernelSizeString = definitions["KERNEL_SIZE"];
		
		noiseSize = 0;
		try {
			// parse the definition to an integer
			noiseSize = std::stoi(noiseSizeString);
			std::cout << "Setting noise size to " + std::to_string(noiseSize) << std::endl;
		}
		catch (const std::invalid_argument& ia) {
			std::cerr << "Can't read noise size for shader " << shaderName << std::endl;
		}

		kernelSize = 0;
		try {
			// parse the definition to an integer
			kernelSize = std::stoi(kernelSizeString);
			std::cout << "Setting kernel size to " + std::to_string(kernelSize) << std::endl;
		}
		catch (const std::invalid_argument& ia) {
			std::cerr << "Can't read kernel size for shader " << shaderName << std::endl;
		}
	}
}