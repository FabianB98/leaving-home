#pragma once

#include <limits>

#include <glm/glm.hpp>

namespace game::components
{
	class AxisConstrainedMoveController
	{
	public:
		AxisConstrainedMoveController() : AxisConstrainedMoveController(glm::vec3(0, 0, 1)) {}

		AxisConstrainedMoveController(glm::vec3 _axis) : AxisConstrainedMoveController(_axis, 1.0f) {}

		AxisConstrainedMoveController(float _mouseWheelSensitivity) 
			: AxisConstrainedMoveController(glm::vec3(0, 0, 1), _mouseWheelSensitivity) {}

		AxisConstrainedMoveController(glm::vec3 _axis, float _mouseWheelSensitivity)
			: AxisConstrainedMoveController(
				_axis, _mouseWheelSensitivity, 
				-std::numeric_limits<float>::infinity(), 
				std::numeric_limits<float>::infinity()
			) {}

		AxisConstrainedMoveController(glm::vec3 _axis, float _mouseWheelSensitivity, float _minValue, float _maxValue)
			: axis(_axis), mouseWheelSensitivity(_mouseWheelSensitivity), minValue(_minValue), maxValue(_maxValue) {}

		glm::vec3 getAxis()
		{
			return axis;
		}

		void setAxis(glm::vec3 _axis)
		{
			axis = _axis;
		}

		float getMouseWheelSensitivity()
		{
			return mouseWheelSensitivity;
		}

		void setMouseWheelSensitivity(float _mouseWheelSensitivity)
		{
			mouseWheelSensitivity = _mouseWheelSensitivity;
		}

		float getMinValue()
		{
			return minValue;
		}

		void setMinValue(float _minValue)
		{
			minValue = _minValue;
		}

		float getMaxValue()
		{
			return maxValue;
		}

		void setMaxValue(float _maxValue)
		{
			maxValue = _maxValue;
		}

	private:
		glm::vec3 axis;
		float mouseWheelSensitivity;

		float minValue;
		float maxValue;
	};
}
