#pragma once

#include <glfw/glfw3.h>

namespace game::components
{
	class FreeFlyingMoveController
	{
	public:
		FreeFlyingMoveController() : FreeFlyingMoveController(5.0f) {}

		FreeFlyingMoveController(float _movementSpeed)
			: FreeFlyingMoveController(
				GLFW_KEY_W,
				GLFW_KEY_S,
				GLFW_KEY_A,
				GLFW_KEY_D,
				GLFW_KEY_LEFT_SHIFT,
				GLFW_KEY_LEFT_CONTROL,
				_movementSpeed
			) {}

		FreeFlyingMoveController(
			int _forwardKey,
			int _backwardKey,
			int _leftKey,
			int _rightKey,
			int _upKey,
			int _downKey,
			float _movementSpeed
		) : forwardKey(_forwardKey),
			backwardKey(_backwardKey),
			leftKey(_leftKey),
			rightKey(_rightKey),
			upKey(_upKey),
			downKey(_downKey),
			movementSpeed(_movementSpeed) {};

		int getForwardKey()
		{
			return forwardKey;
		}

		void setForwardKey(int keyCode)
		{
			forwardKey = keyCode;
		}

		int getBackwardKey()
		{
			return backwardKey;
		}

		void setBackwardKey(int keyCode)
		{
			backwardKey = keyCode;
		}

		int getLeftKey()
		{
			return leftKey;
		}

		void setLeftKey(int keyCode)
		{
			leftKey = keyCode;
		}

		int getRightKey()
		{
			return rightKey;
		}

		void setRightKey(int keyCode)
		{
			rightKey = keyCode;
		}

		int getUpKey()
		{
			return upKey;
		}

		void setUpKey(int keyCode)
		{
			upKey = keyCode;
		}

		int getDownKey()
		{
			return downKey;
		}

		void setDownKey(int keyCode)
		{
			downKey = keyCode;
		}

		float getMovementSpeed()
		{
			return movementSpeed;
		}

		void setMovementSpeed(float _movementSpeed)
		{
			movementSpeed = _movementSpeed;
		}

	private:
		int forwardKey;
		int backwardKey;
		int leftKey;
		int rightKey;
		int upKey;
		int downKey;

		float movementSpeed;
	};
}
