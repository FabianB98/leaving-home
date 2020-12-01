#pragma once

#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <glm/glm.hpp>

namespace std {
	template <>
	struct hash<glm::vec2>
	{
		std::size_t operator()(const glm::vec2& vector) const
		{
			size_t res = 17;
			res = res * 31 + hash<float>()(vector.x);
			res = res * 31 + hash<float>()(vector.y);
			return res;
		}
	};
}

namespace game::world
{
	class Node;
	class DirectedEdge;
	class PlanarGraph;

	class Node
	{
	public:
		Node(glm::vec2 _position) : position(_position), graph(nullptr) {}

		~Node();

		glm::vec2 getPosition()
		{
			return position;
		}

		size_t getNumEdges()
		{
			return edges.size();
		}

		DirectedEdge* getEdge(Node* other);

		const std::unordered_map<glm::vec2, DirectedEdge*> getEdges()
		{
			return edges;
		}

		void addEdgeTo(Node* other);

		void removeEdgeTo(Node* other);

	private:
		glm::vec2 position;

		std::unordered_map<glm::vec2, DirectedEdge*> edges;

		PlanarGraph* graph;

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

		friend std::pair<const glm::vec2, DirectedEdge*>;
		friend std::vector<DirectedEdge*>;
		friend std::unordered_map<glm::vec2, DirectedEdge*>;
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

		Face() = default;
		Face(const Face&) = default;
		Face(Face && edge) = default;

		friend Node;
		friend DirectedEdge;
		friend PlanarGraph;
	};

	class PlanarGraph
	{
	public:
		~PlanarGraph();

		const std::unordered_map<glm::vec2, Node*>& getNodes()
		{
			return nodes;
		}

		size_t getNumNodes()
		{
			return nodes.size();
		}

		Node* getNode(glm::vec2& nodePosition);

		void addNode(Node* node);

		void removeNode(glm::vec2& nodePosition);

		void removeNode(Node* node);

		void addEdge(Node* nodeA, Node* nodeB);

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
		std::unordered_map<glm::vec2, Node*> nodes;

		friend Node;
		friend DirectedEdge;
	};
}
