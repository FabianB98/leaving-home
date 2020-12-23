#include "Camera.hpp"

namespace rendering::components
{
	void Camera::updateViewProjection(glm::mat4 cameraTransform)
	{
		viewMatrix = glm::inverse(cameraTransform);
		viewProjectionMatrix = getProjectionMatrix() * viewMatrix;

		// Extract clipping planes using the Gribb/Hartmann method as explained in https://stackoverflow.com/a/34960913.
		clippingPlanes[0] = normalizePlane(glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 0));	// Left
		clippingPlanes[1] = normalizePlane(glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 0));	// Right
		clippingPlanes[2] = normalizePlane(glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 1));	// Bottom
		clippingPlanes[3] = normalizePlane(glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 1));	// Top
		clippingPlanes[4] = normalizePlane(glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 2));	// Near
		clippingPlanes[5] = normalizePlane(glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 2));	// Far

		position = glm::vec3(cameraTransform[3]);
	}

	glm::vec4 Camera::normalizePlane(glm::vec4 clippingPlane)
	{
		float planeNormalMagnitude = glm::length(glm::vec3(clippingPlane));
		return clippingPlane / planeNormalMagnitude;
	}

	void Camera::applyViewProjection(shading::Shader& shader)
	{
		shader.setUniformVec3("cameraPos", position);

		shader.setUniformVec4("cameraClippingPlanes[0]", clippingPlanes[0]);
		shader.setUniformVec4("cameraClippingPlanes[1]", clippingPlanes[1]);
		shader.setUniformVec4("cameraClippingPlanes[2]", clippingPlanes[2]);
		shader.setUniformVec4("cameraClippingPlanes[3]", clippingPlanes[3]);
		shader.setUniformVec4("cameraClippingPlanes[4]", clippingPlanes[4]);
		shader.setUniformVec4("cameraClippingPlanes[5]", clippingPlanes[5]);
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
