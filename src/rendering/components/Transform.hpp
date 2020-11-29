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
			: translation(glm::vec3(0)), yaw(0), pitch(0), roll(0), scale(glm::vec3(1)), transformChanged(true) {};

		EulerComponentwiseTransform(glm::vec3 _translation, float _yaw, float _pitch, float _roll, glm::vec3 _scale)
			: translation(_translation), yaw(_yaw), pitch(_pitch), roll(_roll), scale(_scale), transformChanged(true) {}

		glm::vec3 getTranslation()
		{
			return translation;
		}

		void setTranslation(glm::vec3 _translation)
		{
			if (_translation != translation)
			{
				transformChanged = true;
				translation = _translation;
			}
		}

		float getYaw()
		{
			return yaw;
		}

		void setYaw(float _yaw)
		{
			if (_yaw != yaw)
			{
				transformChanged = true;
				yaw = _yaw;
			}
		}

		float getPitch()
		{
			return pitch;
		}

		void setPitch(float _pitch)
		{
			if (_pitch != pitch)
			{
				transformChanged = true;
				pitch = _pitch;
			}
		}

		float getRoll()
		{
			return roll;
		}

		void setRoll(float _roll)
		{
			if (_roll != roll)
			{
				transformChanged = true;
				roll = _roll;
			}
		}

		glm::vec3 getScale()
		{
			return scale;
		}

		void setScale(glm::vec3 _scale)
		{
			if (_scale != scale)
			{
				transformChanged = true;
				scale = _scale;
			}
		}

		bool hasTransformChanged()
		{
			return transformChanged;
		}

		glm::mat4 toTransformationMatrix()
		{
			transformChanged = false;
			return glm::translate(translation) * glm::yawPitchRoll(yaw, pitch, roll) * glm::scale(scale);
		}

	private:
		glm::vec3 translation;
		float yaw;
		float pitch;
		float roll;
		glm::vec3 scale;

		bool transformChanged;
	};

	class QuaternionComponentwiseTransform
	{
	public:
		QuaternionComponentwiseTransform()
			: translation(glm::vec3(0)), rotation(glm::quat()), scale(glm::vec3(1)), transformChanged(true) {};

		QuaternionComponentwiseTransform(glm::vec3 _translation, glm::quat _rotation, glm::vec3 _scale)
			: translation(_translation), rotation(_rotation), scale(_scale), transformChanged(true) {}

		glm::vec3 getTranslation()
		{
			return translation;
		}

		void setTranslation(glm::vec3 _translation)
		{
			if (_translation != translation)
			{
				transformChanged = true;
				translation = _translation;
			}
		}

		glm::quat getRotation()
		{
			return rotation;
		}

		void setRotation(glm::quat _rotation)
		{
			if (_rotation != rotation)
			{
				transformChanged = true;
				rotation = _rotation;
			}
		}

		glm::vec3 getScale()
		{
			return scale;
		}

		void setScale(glm::vec3 _scale)
		{
			if (_scale != scale)
			{
				transformChanged = true;
				scale = _scale;
			}
		}

		bool hasTransformChanged()
		{
			return transformChanged;
		}

		glm::mat4 toTransformationMatrix()
		{
			transformChanged = false;
			return glm::translate(translation) * glm::toMat4(rotation) * glm::scale(scale);
		}

	private:
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scale;

		bool transformChanged;
	};
}