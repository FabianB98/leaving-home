#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <glfw/glfw3.h>

namespace gui
{
	void init(GLFWwindow* window);

	void startFrame();

	void render();

	void cleanUp();
}