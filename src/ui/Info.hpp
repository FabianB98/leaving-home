#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>

#include <entt/entt.hpp>

#include "../game/world/World.hpp"
#include "../game/world/buildings/DroneFactoryBuilding.hpp"

namespace gui
{
	void openCellInfo(glm::vec2& mousePos, glm::vec2& frameSize, game::world::Cell* selected);

	void renderCellInfo(entt::registry& registry);
}