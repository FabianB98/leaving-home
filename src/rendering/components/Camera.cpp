#include "Camera.hpp"

namespace rendering::components
{
	void Camera::updateViewProjection(glm::mat4 cameraTransform)
	{
		viewMatrix = glm::inverse(cameraTransform);
		viewProjectionMatrix = getProjectionMatrix() * viewMatrix;

		position = glm::vec3(cameraTransform[3]);
	}

	void Camera::applyViewProjection(shading::Shader& shader)
	{
		shader.setUniformVec3("cameraPos", position);
	}

	float PerspectiveCameraParameters::getFieldOfView()
	{
		return fieldOfView;
	}

	void PerspectiveCameraParameters::setFieldOfView(float _fieldOfView)
	{
		if (_fieldOfView != fieldOfView)
			cameraParametersChanged = true;

		fieldOfView = _fieldOfView;
	}

	float PerspectiveCameraParameters::getAspectRatio()
	{
		return aspectRatio;
	}

	void PerspectiveCameraParameters::setAspectRatio(float _aspectRatio)
	{
		if (_aspectRatio != aspectRatio)
			cameraParametersChanged = true;

		aspectRatio = _aspectRatio;
	}

	float PerspectiveCameraParameters::getNearClippingPlane()
	{
		return nearClippingPlane;
	}

	void PerspectiveCameraParameters::setNearClippingPlane(float _nearClippingPlane)
	{
		if (_nearClippingPlane != nearClippingPlane)
			cameraParametersChanged = true;

		nearClippingPlane = _nearClippingPlane;
	}

	float PerspectiveCameraParameters::getFarClippingPlane()
	{
		return farClippingPlane;
	}

	void PerspectiveCameraParameters::setFarClippingPlane(float _farClippingPlane)
	{
		if (_farClippingPlane != farClippingPlane)
			cameraParametersChanged = true;

		farClippingPlane = _farClippingPlane;
	}

	glm::mat4 PerspectiveCameraParameters::getProjectionMatrix()
	{
		if (cameraParametersChanged)
		{
			projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearClippingPlane, farClippingPlane);
			cameraParametersChanged = false;
		}

		return projectionMatrix;
	}
}
