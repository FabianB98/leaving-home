#include "Camera.hpp"

namespace rendering
{
	float Camera::getFieldOfView()
	{
		return fieldOfView;
	}

	void Camera::setFieldOfView(float _fieldOfView)
	{
		if (_fieldOfView != fieldOfView)
			cameraParametersChanged = true;

		fieldOfView = _fieldOfView;
	}

	float Camera::getAspectRatio()
	{
		return aspectRatio;
	}

	void Camera::setAspectRatio(float _aspectRatio)
	{
		if (_aspectRatio != aspectRatio)
			cameraParametersChanged = true;

		aspectRatio = _aspectRatio;
	}

	void Camera::setAspectRatio(float width, float height)
	{
		setAspectRatio(width / height);
	}

	float Camera::getNearClippingPlane()
	{
		return nearClippingPlane;
	}

	void Camera::setNearClippingPlane(float _nearClippingPlane)
	{
		if (_nearClippingPlane != nearClippingPlane)
			cameraParametersChanged = true;

		nearClippingPlane = _nearClippingPlane;
	}

	float Camera::getFarClippingPlane()
	{
		return farClippingPlane;
	}

	void Camera::setFarClippingPlane(float _farClippingPlane)
	{
		if (_farClippingPlane != farClippingPlane)
			cameraParametersChanged = true;

		farClippingPlane = _farClippingPlane;
	}

	glm::mat4 Camera::getProjectionMatrix()
	{
		if (cameraParametersChanged)
		{
			projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearClippingPlane, farClippingPlane);
			cameraParametersChanged = false;
		}

		return projectionMatrix;
	}
}
