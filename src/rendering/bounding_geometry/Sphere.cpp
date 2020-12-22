#include "Sphere.hpp"

#include <stdio.h>
#include <iostream>

namespace rendering::bounding_geometry
{
	Sphere::Sphere(const std::vector<glm::vec3>& vertices, DefinitionSpace* _definitionSpace) :
		center(glm::vec3(0.0f)),
		radius(0.0f),
		definitionSpace(_definitionSpace)
	{
		fitToVertices(vertices);
	}

	void Sphere::fitToVertices(const std::vector<glm::vec3>& vertices) {
		// Ritter's bounding sphere as explained in https://en.wikipedia.org/wiki/Bounding_sphere#Ritter.27s_bounding_sphere.
		// This algorithm is not optimal. It only produces a "coarse result which is usually 5% to 20% larger than the optimum."

		// Pick a point x from P (here: vertices).
		std::default_random_engine generator;
		std::uniform_int_distribution<size_t> dist(0, vertices.size() - 1);
		size_t index = dist(generator);
		glm::vec3 x = vertices[index];

		// Search a point y in P (here: vertices) which has the largest distance from x and search a point z in P (here:
		// vertices) which has the largest distance from y.
		glm::vec3 y = furthestPointAwayFrom(vertices, x);
		glm::vec3 z = furthestPointAwayFrom(vertices, y);

		// Set up an initial ball B (here: this Sphere instance) with its centre as the midpoint of y and z, the radius as half
		// of the distance between y and z.
		center = (y + z) / 2.0f;
		radius = glm::distance(y, z) / 2.0f;

		// If all points in P (here: vertices) are within ball B (here: this Sphere instance), then we get a bounding sphere.
		// Otherwise, let p be a point outside the ball, construct a new ball covering both point p and previous ball. Repeat this
		// step until all points are covered.
		float squaredRadius = radius * radius;
		for (glm::vec3 p : vertices)
		{
			float distanceSquared = glm::distance2(center, p);
			if (distanceSquared > squaredRadius)
				squaredRadius = distanceSquared;
		}
		radius = std::sqrt(squaredRadius);
	}

	glm::vec3 Sphere::furthestPointAwayFrom(const std::vector<glm::vec3>& vertices, const glm::vec3& p)
	{
		glm::vec3 result = p;
		float currentMaxDistanceSquared = 0.0f;

		for (glm::vec3 vertex : vertices)
		{
			float distanceSquared = glm::distance2(vertex, p);
			if (distanceSquared > currentMaxDistanceSquared)
			{
				result = vertex;
				currentMaxDistanceSquared = distanceSquared;
			}
		}

		return result;
	}

	bool Sphere::isInCameraFrustum(const std::array<glm::vec4, 6>& clippingPlanes, const glm::mat4& modelMatrix)
	{
		// Translate, rotate and scale the sphere according to the model matrix.
		std::pair<glm::vec3, float> transformedParams = definitionSpace->convertToWorldSpace(*this, modelMatrix);
		glm::vec3& transformedCenter = transformedParams.first;
		float& transformedRadius = transformedParams.second;

		// Sphere frustum intersection test as described in 
		// https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
		bool inFrustum = true;

		for (int i = 0; i < 6; i++)
			if (glm::dot(glm::vec3(clippingPlanes[i]), transformedCenter) + clippingPlanes[i].w <= -transformedRadius)
				inFrustum = false;

		return inFrustum;
	}

	std::pair<glm::vec3, float> Sphere::WorldSpace::convertToWorldSpace(Sphere& sphere, const glm::mat4& modelMatrix)
	{
		return std::make_pair(sphere.center, sphere.radius);
	}

	std::pair<glm::vec3, float> Sphere::ObjectSpace::convertToWorldSpace(Sphere& sphere, const glm::mat4& modelMatrix)
	{
		glm::vec4 centerWorldSpaceHomogenous = modelMatrix * glm::vec4(sphere.center, 1);
		glm::vec3 centerWorldSpace = glm::vec3(centerWorldSpaceHomogenous) / centerWorldSpaceHomogenous.w;

		float scaleFactor1 = glm::length2(glm::vec3(modelMatrix[0]));
		float scaleFactor2 = glm::length2(glm::vec3(modelMatrix[1]));
		float scaleFactor3 = glm::length2(glm::vec3(modelMatrix[2]));
		float scaledRadius = sphere.radius * sqrt(std::max(scaleFactor1, std::max(scaleFactor2, scaleFactor3)));

		return std::make_pair(centerWorldSpace, scaledRadius);
	}
}
