#include "PlanarGraph.hpp"

namespace game::world
{
	Node::~Node()
	{
		while (!edges.empty())
		{
			DirectedEdge* edge = edges.begin()->second;
			delete edge;
		}

		if (graph != nullptr)
			graph->nodes.erase(position);
	}

	void Node::setPosition(glm::vec2 _position)
	{
		glm::vec2 oldPosition = position;
		_position = position;

		if (graph != nullptr)
		{
			graph->nodes.erase(oldPosition);
			graph->nodes.insert(std::make_pair(position, this));
		}
	}

	DirectedEdge* Node::getEdge(Node* other)
	{
		auto findResult = edges.find(other);
		if (findResult != edges.end())
			return findResult->second;
		else
			return nullptr;
	}

	std::pair<DirectedEdge*, DirectedEdge*> Node::addEdgeTo(Node* other)
	{
		std::pair<DirectedEdge*, DirectedEdge*> edges = DirectedEdge::createEdge(this, other);

		addEdge(edges.first);
		other->addEdge(edges.second);

		return edges;
	}

	void Node::addEdge(DirectedEdge* edge)
	{
		DirectedEdge* nextClockwise;
		DirectedEdge* nextCounterclockwise;
		if (edges.size() == 0)
		{
			nextClockwise = edge;
			nextCounterclockwise = edge;
		}
		else if (edges.size() == 1)
		{
			nextClockwise = edges.begin()->second;
			nextCounterclockwise = edges.begin()->second;
		}
		else
		{
			nextClockwise = edges.begin()->second;
			nextCounterclockwise = nextClockwise->nextCounterclockwise;

			float angle = edge->getAngle();
			float nextClockwiseAngle = nextClockwise->getAngle();
			float nextCounterclockwiseAngle = nextCounterclockwise->getAngle();

			while (!(nextClockwiseAngle < angle && angle < nextCounterclockwiseAngle)
				&& !(nextClockwiseAngle > nextCounterclockwiseAngle && (nextClockwiseAngle < angle || angle < nextCounterclockwiseAngle)))
			{
				nextClockwise = nextCounterclockwise;
				nextCounterclockwise = nextClockwise->nextCounterclockwise;

				nextClockwiseAngle = nextCounterclockwiseAngle;
				nextCounterclockwiseAngle = nextCounterclockwise->getAngle();
			}
		}

		edge->nextClockwise = nextClockwise;
		edge->nextCounterclockwise = nextCounterclockwise;
		nextClockwise->nextCounterclockwise = edge;
		nextCounterclockwise->nextClockwise = edge;

		edges.insert(std::make_pair(edge->to, edge));
	}

	void Node::removeEdgeTo(Node* other)
	{
		DirectedEdge* edge = getEdge(other);
		if (edge != nullptr)
			delete edge;
	}

	std::pair<DirectedEdge*, DirectedEdge*> DirectedEdge::createEdge(Node* nodeA, Node* nodeB)
	{
		DirectedEdge* forwardEdge = new DirectedEdge(nodeA, nodeB);
		DirectedEdge* backwardEdge = new DirectedEdge(nodeB, nodeA);

		forwardEdge->nextClockwise = forwardEdge;
		forwardEdge->nextCounterclockwise = forwardEdge;
		forwardEdge->otherDirection = backwardEdge;

		backwardEdge->nextClockwise = backwardEdge;
		backwardEdge->nextCounterclockwise = backwardEdge;
		backwardEdge->otherDirection = forwardEdge;

		return std::make_pair(forwardEdge, backwardEdge);
	}

	DirectedEdge::~DirectedEdge()
	{
		to->edges.erase(from);
		from->edges.erase(to);

		if (nextClockwise != nullptr && nextCounterclockwise != nullptr)
		{
			nextClockwise->nextCounterclockwise = nextCounterclockwise;
			nextCounterclockwise->nextClockwise = nextClockwise;
		}

		if (otherDirection != nullptr)
		{
			otherDirection->otherDirection = nullptr;
			delete otherDirection;
			otherDirection = nullptr;
		}
	}

	float DirectedEdge::getAngle()
	{
		glm::vec2 direction = glm::normalize(to->position - from->position);

		if (direction.y >= 0.0f)
			return acos(direction.x);
		else
			return -acos(direction.x);
	}

	Face DirectedEdge::calculateFace()
	{
		Face face = Face();

		DirectedEdge* edge = this;
		do
		{
			face.nodes.push_back(edge->from);
			face.edges.push_back(edge);

			edge = edge->otherDirection->nextCounterclockwise;
		} while (edge != this);

		return face;
	}

	PlanarGraph::~PlanarGraph()
	{
		while (!nodes.empty())
		{
			Node* node = nodes.begin()->second;
			delete node;
		}
	}

	Node* PlanarGraph::getNodeAt(glm::vec2 position)
	{
		auto findResult = nodes.find(position);
		if (findResult != nodes.end())
			return findResult->second;
		else
			return nullptr;
	}

	void PlanarGraph::addNode(Node* node)
	{
		if (node->graph == nullptr)
		{
			node->graph = this;
			nodes.insert(std::make_pair(node->position, node));
		}
	}

	void PlanarGraph::removeNode(Node* node)
	{
		if (nodes.find(node->position) != nodes.end())
		{
			delete node;
		}
	}

	std::pair<DirectedEdge*, DirectedEdge*> PlanarGraph::addEdge(Node* nodeA, Node* nodeB)
	{
		addNode(nodeA);
		addNode(nodeB);

		return nodeA->addEdgeTo(nodeB);
	}

	std::vector<Face*> PlanarGraph::calculateFaces()
	{
		std::vector<Face*> faces;

		std::unordered_set<DirectedEdge*> traversedEdges;
		for (auto& node : nodes)
		{
			for (auto& outgoingEdge : node.second->edges)
			{
				if (traversedEdges.find(outgoingEdge.second) == traversedEdges.end())
				{
					Face* face = new Face(outgoingEdge.second->calculateFace());
					faces.push_back(face);

					for (auto& edge : face->edges)
						traversedEdges.insert(edge);
				}
			}
		}

		return faces;
	}
}
