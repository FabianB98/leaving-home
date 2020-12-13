#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <glfw/glfw3.h>

#include "Util.hpp"
#include "../rendering/textures/Texture.hpp"
#include "../game/DayNightCycle.hpp"

namespace gui
{
	enum class CameraType
	{
		DEFAULT,
		FREE
	};

	void renderDebugWindow(game::DayNightCycle& daynight, CameraType* camera);
}