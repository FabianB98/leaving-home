#include "Lights.hpp"

namespace rendering
{
	// Directional Light
	glm::vec3 DirectionalLight::getIntensity() const { return intensity; }

	void DirectionalLight::setIntensity(glm::vec3& _intensity) { intensity = _intensity; }

	glm::vec3 DirectionalLight::getDirection() const { return direction; }

	void DirectionalLight::setDirection(glm::vec3& _direction) { direction = _direction; }

	// Point Light
	glm::vec3 PointLight::getIntensity() const { return intensity; }

	void PointLight::setIntensity(glm::vec3& _intensity) { intensity = _intensity; }

	glm::vec3 PointLight::getPosition() const { return position; }

	void PointLight::setPosition(glm::vec3& _position) { position = _position; }
}