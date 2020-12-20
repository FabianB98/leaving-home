#include "Sphere.hpp"

namespace rendering::bounding_geometry
{
	Sphere::Sphere(std::vector<glm::vec3> vertices)
	{
		std::unordered_map<size_t, glm::vec3> P;
		for (size_t i = 0; i < vertices.size(); i++)
			P.insert(std::make_pair(i, vertices[i]));

		std::pair<glm::vec3, float> ball = smallestEnclosingBallWithPoints(P, std::unordered_map<size_t, glm::vec3>());
		center = ball.first;
		radius = ball.second;
	}

	std::pair<glm::vec3, float> Sphere::smallestEnclosingBallWithPoints(
		std::unordered_map<size_t, glm::vec3>& P,
		std::unordered_map<size_t, glm::vec3>& Q
	) {
		// Welzls algorithm as explained on slide 12-51 of the "Algorithmen II" lecture from the winter term 2018/2019.
		// See https://youtu.be/u9wEjKEzCjw?t=5 for a recording of that part of the lecture (in German).
		if (P.size() == 1 || Q.size() == 4)
		{
			// Base case. The ball can be calculated directly with the given points.
			glm::vec3 ballCenter = glm::vec3(0.0f);
			for (auto& point : Q)
				ballCenter += point.second;
			ballCenter /= (float)Q.size();

			float ballRadius = 0.0f;
			for (auto& point : Q)
				ballRadius = std::max(ballRadius, glm::distance(ballCenter, point.second));

			return std::make_pair(ballCenter, ballRadius);
		}

		// Pick random x element P.
		std::default_random_engine generator;
		size_t maxElementInP = P.size() - 1;
		std::uniform_int_distribution<size_t> dist(0, maxElementInP);
		size_t index = dist(generator);
		glm::vec3 x = P[index];

		// Temporarily use P as (P \ {x}).
		P[index] = P[maxElementInP];
		P.erase(maxElementInP);

		std::pair<glm::vec3, float> B = smallestEnclosingBallWithPoints(P, Q);

		if (glm::distance(B.first, x) <= B.second)
		{
			// x is an element of B.
			// Insert x into (P \ {x}) to reset P into the state it was in before this method was called.
			P.insert(std::make_pair(maxElementInP, x));

			return B;
		}

		// Temporarily use Q as (Q union {x}).
		size_t maxElementInQ = Q.size();
		Q.insert(std::make_pair(maxElementInQ, x));

		B = smallestEnclosingBallWithPoints(P, Q);

		// Reset P and Q into the state they were in before this method was called.
		Q.erase(maxElementInQ);
		P.insert(std::make_pair(maxElementInP, x));

		return B;
	}

	bool Sphere::isInCameraFrustum(std::array<glm::vec4, 6> clippingPlanes, glm::mat4 modelMatrix)
	{
		// Translate, rotate and scale the sphere according to the model matrix.
		glm::vec4 centerWorldSpaceHomogenous = modelMatrix * glm::vec4(center, 1);
		glm::vec3 centerWorldSpace = glm::vec3(centerWorldSpaceHomogenous) / centerWorldSpaceHomogenous.w;

		float scaleFactor1 = glm::length2(glm::vec3(modelMatrix[0]));
		float scaleFactor2 = glm::length2(glm::vec3(modelMatrix[1]));
		float scaleFactor3 = glm::length2(glm::vec3(modelMatrix[2]));
		float scaledRadius = radius * sqrt(std::max(scaleFactor1, std::max(scaleFactor2, scaleFactor3)));

		// Sphere frustum intersection test as described in 
		// https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
		bool inFrustum = true;

		for (int i = 0; i < 6; i++)
			if (glm::dot(glm::vec3(clippingPlanes[i]), centerWorldSpace) + clippingPlanes[i].w <= -scaledRadius)
				inFrustum = false;

		return inFrustum;
	}
}
