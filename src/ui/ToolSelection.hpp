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

	enum class Building
	{
		TEST_BUILDING,
		OTHER_TEST_BUILDING
	};

	void initToolSelection();

	void renderToolSelection(Tool* selected, Building* selectedBuilding, float frameHeight);

	void cleanUpToolSelection();
}