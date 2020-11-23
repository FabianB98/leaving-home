#pragma once

#include <glm/glm.hpp>

namespace game::components
{
	class FirstPersonRotateController
	{
	public:
		FirstPersonRotateController() : FirstPersonRotateController(0.2f) {}

		FirstPersonRotateController(int _mouseButtonCode) : FirstPersonRotateController(_mouseButtonCode, 0.2f) {}

		FirstPersonRotateController(float _mouseSensitivity) 
			: FirstPersonRotateController(-1, _mouseSensitivity) {}

		FirstPersonRotateController(int _mouseButtonCode, float _mouseSensitivity)
			: FirstPersonRotateController(_mouseButtonCode, _mouseSensitivity, glm::radians(-90.0), glm::radians(90.0)) {}

		FirstPersonRotateController(int _mouseButtonCode, float _mouseSensitivity, float _minPitch, float _maxPitch)
			: mouseButtonCode(_mouseButtonCode), mouseSensitivity(_mouseSensitivity), minPitch(_minPitch), maxPitch(_maxPitch) {}

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

		float getMinPitch()
		{
			return minPitch;
		}

		void setMinPitch(float _minPitch)
		{
			minPitch = _minPitch;
		}

		float getMaxPitch()
		{
			return maxPitch;
		}

		void setMaxPitch(float _maxPitch)
		{
			maxPitch = _maxPitch;
		}

	private:
		int mouseButtonCode;

		float mouseSensitivity;

		float minPitch;
		float maxPitch;
	};
}
