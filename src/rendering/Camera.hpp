#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace rendering
{
	class Camera
	{
	public:
		Camera(float _fieldOfView, float _aspectRatio, float _nearClippingPlane, float _farClippingPlane) :
			fieldOfView(_fieldOfView), aspectRatio(_aspectRatio),
			nearClippingPlane(_nearClippingPlane), farClippingPlane(_farClippingPlane) {};

		float getFieldOfView();

		void setFieldOfView(float _fieldOfView);

		float getAspectRatio();

		void setAspectRatio(float _aspectRatio);

		void setAspectRatio(float width, float height);

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

		glm::mat4 projectionMatrix;
		bool cameraParametersChanged = true;
	};
}
