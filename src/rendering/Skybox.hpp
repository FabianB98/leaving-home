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

// Headers related to the entity component system.
#include <entt/entt.hpp>

// our headers
#include "model/Mesh.hpp"
#include "RenderingEngine.hpp"
#include "components/Camera.hpp"
#include "components/Transform.hpp"
#include "textures/Texture.hpp"

namespace rendering
{
	class Skybox
	{
	public:
		Skybox(std::string cubemapName, std::string shaderName);

		void use();

		void render(RenderingEngine* engine, GLuint defaultDepthFunc = GL_LESS);

		std::shared_ptr<shading::Shader> getShader()
		{
			return shader;
		}

	private:
		std::shared_ptr<model::Mesh> cube;
		std::shared_ptr<shading::Shader> shader;
		std::shared_ptr<textures::Cubemap> cubemap;
	};
}