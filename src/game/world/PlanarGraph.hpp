#pragma once

#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <glm/glm.hpp>

#define NODE_POS_ROUNDING_PRECISION 0.001f

namespace std
{
	template <>
	struct hash<glm::vec2>
	{
		std::size_t operator()(const glm::vec2& vec) const
		{
			size_t res = 17;
			res = res * 31 + hash<float>()(vec.x);
			res = res * 31 + hash<float>()(vec.y);
			return res;
		}
	};
}

// floor, ceil and round functions from the standard library are extremely slow for some reason, so we're going to
// implement a faster round function by ourselves.
static inline long fastRound(const float value)
{
	float valueOffset = value + 0.5f;
	long l = (long)valueOffset;
	return l - (l > valueOffset);
}

class Hasher
{
public:
	std::size_t operator()(const glm::vec2& vector) const
	{
		size_t res = 17;
		res = res * 31 + std::hash<long>()(fastRound(vector.x * NODE_POS_ROUNDING_PRECISION));
		res = res * 31 + std::hash<long>()(fastRound(vector.y * NODE_POS_ROUNDING_PRECISION));
		return res;
	}
};

class EqualFn
{
public:
	bool operator()(const glm::vec2& a, const glm::vec2& b) const
	{
		return fastRound(a.x / NODE_POS_ROUNDING_PRECISION) == fastRound(b.x / NODE_POS_ROUNDING_PRECISION)
			&& fastRound(a.y / NODE_POS_ROUNDING_PRECISION) == fastRound(b.y / NODE_POS_ROUNDING_PRECISION);
	}
};

namespace game::world
{
	class Node;
	class DirectedEdge;
	class Face;
	class PlanarGraph;

	class Node
	{
	public:
		Node(glm::vec2 _position) : position(_position), graph(nullptr), additionalData(nullptr) {}

		~Node();

		glm::vec2 getPosition()
		{
			return position;
		}

		void setPosition(glm::vec2 _position);

		size_t getNumEdges()
		{
			return edges.size();
		}

		DirectedEdge* getEdge(Node* other);

		const std::unordered_map<Node*, DirectedEdge*> getEdges()
		{
			return edges;
		}

		std::pair<DirectedEdge*, DirectedEdge*> addEdgeTo(Node* other);

		void removeEdgeTo(Node* other);

		void setAdditionalData(void* _additionalData)
		{
			additionalData = _additionalData;
		}

		void* getAdditionalData()
		{
			return additionalData;
		}

	private:
		glm::vec2 position;

		std::unordered_map<Node*, DirectedEdge*> edges;

		PlanarGraph* graph;

		void* additionalData;

		Node() = default;
		Node(const Node&) = default;
		Node(Node && edge) = default;

		void addEdge(DirectedEdge* edge);

		friend DirectedEdge;
		friend PlanarGraph;
	};

	class DirectedEdge
	{
	public:
		static std::pair<DirectedEdge*, DirectedEdge*> createEdge(Node* nodeA, Node* nodeB);

		~DirectedEdge();

		Node* getFrom()
		{
			return from;
		}

		Node* getTo()
		{
			return to;
		}

		DirectedEdge* getNextClockwise()
		{
			return nextClockwise;
		}

		DirectedEdge* getNextCounterclockwise()
		{
			return nextCounterclockwise;
		}

		DirectedEdge* getOtherDirection()
		{
			return otherDirection;
		}

		Face calculateFace();

	private:
		Node* from;
		Node* to;

		DirectedEdge* nextClockwise;
		DirectedEdge* nextCounterclockwise;
		DirectedEdge* otherDirection;

		DirectedEdge() = default;
		DirectedEdge(const DirectedEdge&) = default;
		DirectedEdge(DirectedEdge&& edge) = default;

		DirectedEdge(Node* _from, Node* _to)
			: from(_from), to(_to), nextClockwise(nullptr), nextCounterclockwise(nullptr), otherDirection(nullptr) {};

		float getAngle();

		friend Node;
		friend PlanarGraph;

		friend std::pair<const Node*, DirectedEdge*>;
		friend std::vector<DirectedEdge*>;
		friend std::unordered_map<Node*, DirectedEdge*>;
	};

	class Face
	{
	public:
		size_t getNumNodes()
		{
			return nodes.size();
		}

		const std::vector<Node*> getNodes()
		{
			return nodes;
		}

		size_t getNumEdges()
		{
			return edges.size();
		}

		const std::vector<DirectedEdge*> getEdges()
		{
			return edges;
		}

	private:
		std::vector<Node*> nodes;
		std::vector<DirectedEdge*> edges;

		friend Node;
		friend DirectedEdge;
		friend PlanarGraph;
	};

	class PlanarGraph
	{
	public:
		~PlanarGraph();

		Node* getNodeAt(glm::vec2 position);

		const std::unordered_map<glm::vec2, Node*, Hasher, EqualFn>& getNodes()
		{
			return nodes;
		}

		size_t getNumNodes()
		{
			return nodes.size();
		}

		void addNode(Node* node);

		void removeNode(Node* node);

		std::pair<DirectedEdge*, DirectedEdge*> addEdge(Node* nodeA, Node* nodeB);

		void removeEdge(DirectedEdge* edge)
		{
			edge->getFrom()->removeEdgeTo(edge->getTo());
		}

		void removeEdge(Node* nodeA, Node* nodeB)
		{
			nodeA->removeEdgeTo(nodeB);
		}

		std::vector<Face*> calculateFaces();

	private:
		std::unordered_map<glm::vec2, Node*, Hasher, EqualFn> nodes;

		friend Node;
		friend DirectedEdge;
	};
}
