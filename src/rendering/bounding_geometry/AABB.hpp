#pragma once

#include "BoundingGeometry.hpp"

namespace rendering::bounding_geometry
{
	class AABB : public BoundingGeometry
	{
	public:
		class DefinitionSpace;

		AABB(DefinitionSpace* _definitionSpace) : AABB(glm::vec3(0.0f), glm::vec3(0.0f), _definitionSpace) {}

		AABB(glm::vec3 _min, glm::vec3 _max, DefinitionSpace* _definitionSpace) : 
			min(_min),
			max(_max),
			definitionSpace(_definitionSpace) {}

		AABB(const std::vector<glm::vec3>& vertices, DefinitionSpace* _definitionSpace);

		~AABB()
		{
			delete definitionSpace;
		}

		void fitToVertices(const std::vector<glm::vec3>& vertices);

		void extendToFit(const std::vector<std::shared_ptr<BoundingGeometry>> boundingGeometries);

		std::vector<glm::vec3> getExtremaPoints();

		bool isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix);

		glm::vec3 getMin()
		{
			return min;
		}

		glm::vec3 getMax()
		{
			return max;
		}

		class DefinitionSpace {
		private:
			virtual std::pair<glm::vec3, glm::vec3> convertToWorldSpace(AABB& aabb, const glm::mat4& modelMatrix) = 0;

			friend AABB;
		};

		class WorldSpace : public DefinitionSpace {
		private:
			std::pair<glm::vec3, glm::vec3> convertToWorldSpace(AABB& aabb, const glm::mat4& modelMatrix);

			friend AABB;
		};

		class ObjectSpace : public DefinitionSpace {
		private:
			std::pair<glm::vec3, glm::vec3> convertToWorldSpace(AABB& aabb, const glm::mat4& modelMatrix);

			friend AABB;
		};		

	private:
		glm::vec3 min;
		glm::vec3 max;

		DefinitionSpace* definitionSpace;
	};
}
