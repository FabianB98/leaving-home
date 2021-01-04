#pragma once

#include <array>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "../shading/Shader.hpp"

namespace rendering::components
{
	class CameraParameters
	{
	public:
		virtual void setAspectRatio(float _aspectRatio) = 0;

		virtual glm::mat4 getProjectionMatrix() = 0;

	protected:
		glm::mat4 projectionMatrix{ glm::mat4(1.f) };
		bool cameraParametersChanged = true;
	};

	class OrthographicCameraParameters : public CameraParameters
	{
	public:
		OrthographicCameraParameters(float _width, float _aspectRatio, float _nearClippingPlane, float _farClippingPlane) :
			width(_width), height(_width / _aspectRatio), aspectRatio(_aspectRatio), nearClippingPlane(_nearClippingPlane), farClippingPlane(_farClippingPlane) {};

		float getWidth();

		void setWidth(float _width);

		float getHeight();

		void setAspectRatio(float _aspectRatio);

		float getNearClippingPlane();

		void setNearClippingPlane(float _nearClippingPlane);

		float getFarClippingPlane();

		void setFarClippingPlane(float _farClippingPlane);

		glm::mat4 getProjectionMatrix();
	private:
		float width;
		float height;
		float aspectRatio;
		float nearClippingPlane;
		float farClippingPlane;
	};

	class PerspectiveCameraParameters : public CameraParameters
	{
	public:
		PerspectiveCameraParameters(float _fieldOfView, float _aspectRatio, float _nearClippingPlane, float _farClippingPlane) :
			fieldOfView(_fieldOfView), aspectRatio(_aspectRatio),
			nearClippingPlane(_nearClippingPlane), farClippingPlane(_farClippingPlane) {};

		float getFieldOfView();

		void setFieldOfView(float _fieldOfView);

		float getAspectRatio();

		void setAspectRatio(float _aspectRatio);

		float getNearClippingPlane();

		void setNearClippingPlane(float _nearClippingPlane);

		float getFarClippingPlane();

		void setFarClippingPlane(float _farClippingPlane);

		glm::mat4 getProjectionMatrix();

	private:
		float fieldOfView;
		float aspectRatio;
		float nearClippingPlane;
		float farClippingPlane;
	};

	class Camera
	{
	public:
		Camera(std::shared_ptr<CameraParameters> _parameters) : parameters(std::move(_parameters)) {};

		void setAspectRatio(float _aspectRatio)
		{
			parameters->setAspectRatio(_aspectRatio);
		}

		void setAspectRatio(float width, float height)
		{
			setAspectRatio(width / height);
		}

		glm::mat4 getProjectionMatrix()
		{
			return parameters->getProjectionMatrix();
		}

		void updateViewProjection(glm::mat4 cameraTransform);

		void applyViewProjection(shading::Shader& shader);

		glm::mat4 getViewMatrix()
		{
			return viewMatrix;
		}

		glm::mat4 getViewProjectionMatrix()
		{
			return viewProjectionMatrix;
		}

		glm::vec3 getPosition()
		{
			return position;
		}

		const std::array<glm::vec4, 6>& getClippingPlanes()
		{
			return clippingPlanes;
		}

	private:
		std::shared_ptr<CameraParameters> parameters;

		glm::vec3 position{};
		glm::mat4 viewMatrix{ glm::mat4(1.f) };
		glm::mat4 viewProjectionMatrix{ glm::mat4(1.f) };

		// Clipping planes are ordered as follows: Left clipping plane, right clipping plane, bottom clipping plane,
		// top clipping plane, near clipping plane, far clipping plane.
		std::array<glm::vec4, 6> clippingPlanes{ 
			glm::vec4(0.f), glm::vec4(0.f), glm::vec4(0.f), glm::vec4(0.f), glm::vec4(0.f), glm::vec4(0.f) };

		glm::vec4 normalizePlane(glm::vec4 clippingPlane);
	};
}
