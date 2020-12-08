#pragma once

#include <glfw/glfw3.h>

namespace game::components
{
	class HeightConstrainedMoveController
	{
	public:
		HeightConstrainedMoveController() : HeightConstrainedMoveController(GLFW_MOUSE_BUTTON_RIGHT) {}

		HeightConstrainedMoveController(float _mouseSensitivity) 
			: HeightConstrainedMoveController(GLFW_MOUSE_BUTTON_RIGHT, _mouseSensitivity) {}

		HeightConstrainedMoveController(int _mouseButtonCode)
			: HeightConstrainedMoveController(_mouseButtonCode, 1.0f) {}

		HeightConstrainedMoveController(int _mouseButtonCode, float _mouseSensitivity) 
			: mouseButtonCode(_mouseButtonCode), mouseSensitivity(_mouseSensitivity) {}

		int getMouseButtonCode()
		{
			return mouseButtonCode;
		}

		void setMouseButtonCode(int _mouseButtonCode)
		{
			mouseButtonCode = _mouseButtonCode;
		}

		float getMouseSensitivity() 
		{
			return mouseSensitivity;
		}

		void setMouseSensitivity(float _mouseSensitivity)
		{
			mouseSensitivity = _mouseSensitivity;
		}

	private:
		int mouseButtonCode;
		float mouseSensitivity;
	};
}
