#pragma once

#include <algorithm>
#include <math.h>
#include <random>
#include <unordered_map>

#include <glm/gtx/norm.hpp>

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class Sphere : public BoundingGeometry
	{
	public:
		class DefinitionSpace;

		Sphere(DefinitionSpace* _definitionSpace) : Sphere(glm::vec3(0.0f), 0.0f, _definitionSpace) {}

		Sphere(glm::vec3 _center, float _radius, DefinitionSpace* _definitionSpace) : 
			center(_center),
			radius(_radius),
			definitionSpace(_definitionSpace) {}

		Sphere(const std::vector<glm::vec3>& vertices, DefinitionSpace* _definitionSpace);

		~Sphere()
		{
			delete definitionSpace;
		}

		void fitToVertices(const std::vector<glm::vec3>& vertices);

		void extendToFitGeometries(const std::vector<std::shared_ptr<BoundingGeometry>> boundingGeometries);

		std::vector<glm::vec3> getExtremaPoints();

		std::shared_ptr<BoundingGeometry> toWorldSpace(const glm::mat4& modelMatrix);

		bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix);

		glm::vec3 getCenter()
		{
			return center;
		}

		float getRadius()
		{
			return radius;
		}

		class DefinitionSpace {
		private:
			virtual std::pair<glm::vec3, float> convertToWorldSpace(Sphere& sphere, const glm::mat4& modelMatrix) = 0;

			friend Sphere;
		};

		class WorldSpace : public DefinitionSpace {
		private:
			std::pair<glm::vec3, float> convertToWorldSpace(Sphere& sphere, const glm::mat4& modelMatrix);

			friend Sphere;
		};

		class ObjectSpace : public DefinitionSpace {
		private:
			std::pair<glm::vec3, float> convertToWorldSpace(Sphere& sphere, const glm::mat4& modelMatrix);

			friend Sphere;
		};

	private:
		glm::vec3 center;
		float radius;

		DefinitionSpace* definitionSpace;

		glm::vec3 furthestPointAwayFrom(const std::vector<glm::vec3>& vertices, const glm::vec3& p);
	};
}
