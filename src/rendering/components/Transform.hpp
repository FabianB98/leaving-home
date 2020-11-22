#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace rendering::components
{
	class MatrixTransform
	{
	public:
		MatrixTransform(glm::mat4 _transform) : transform(_transform) {}

		glm::mat4 getTransform()
		{
			return transform;
		}

		void setTransform(glm::mat4 _transform)
		{
			transform = _transform;
		}

	private:
		glm::mat4 transform{ glm::mat4(1.f) };
	};

	class EulerComponentwiseTransform
	{
	public:
		EulerComponentwiseTransform()
			: translation(glm::vec3(0)), yaw(0), pitch(0), roll(0), scale(glm::vec3(1)) {};

		EulerComponentwiseTransform(glm::vec3 _translation, float _yaw, float _pitch, float _roll, glm::vec3 _scale)
			: translation(_translation), yaw(_yaw), pitch(_pitch), roll(_roll), scale(_scale) {}

		glm::vec3 getTranslation()
		{
			return translation;
		}

		void setTranslation(glm::vec3 _translation)
		{
			translation = _translation;
		}

		float getYaw()
		{
			return yaw;
		}

		void setYaw(float _yaw)
		{
			yaw = _yaw;
		}

		float getPitch()
		{
			return pitch;
		}

		void setPitch(float _pitch)
		{
			pitch = _pitch;
		}

		float getRoll()
		{
			return roll;
		}

		void setRoll(float _roll)
		{
			roll = _roll;
		}

		glm::vec3 getScale()
		{
			return scale;
		}

		void setScale(glm::vec3 _scale)
		{
			scale = _scale;
		}

		glm::mat4 toTransformationMatrix()
		{
			return glm::translate(translation) * glm::yawPitchRoll(yaw, pitch, roll) * glm::scale(scale);
		}

	private:
		glm::vec3 translation;
		float yaw;
		float pitch;
		float roll;
		glm::vec3 scale;
	};

	class QuaternionComponentwiseTransform
	{
	public:
		QuaternionComponentwiseTransform()
			: translation(glm::vec3(0)), rotation(glm::quat()), scale(glm::vec3(1)) {};

		QuaternionComponentwiseTransform(glm::vec3 _translation, glm::quat _rotation, glm::vec3 _scale)
			: translation(_translation), rotation(_rotation), scale(_scale) {}

		glm::vec3 getTranslation()
		{
			return translation;
		}

		void setTranslation(glm::vec3 _translation)
		{
			translation = _translation;
		}

		glm::quat getRotation()
		{
			return rotation;
		}

		void setRotation(glm::quat _rotation)
		{
			rotation = _rotation;
		}

		glm::vec3 getScale()
		{
			return scale;
		}

		void setScale(glm::vec3 _scale)
		{
			scale = _scale;
		}

		glm::mat4 toTransformationMatrix()
		{
			return glm::translate(translation) * glm::toMat4(rotation) * glm::scale(scale);
		}

	private:
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scale;
	};
}