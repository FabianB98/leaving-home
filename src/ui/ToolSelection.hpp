#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <glfw/glfw3.h>

#include "Util.hpp"
#include "../rendering/textures/Texture.hpp"

namespace gui
{
	enum class Tool 
	{
		BUILD,
		VIEW,
		REMOVE
	};

	void initToolSelection();

	void renderToolSelection(Tool* selected, float frameHeight);

	void cleanUpToolSelection();
}