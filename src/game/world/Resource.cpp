#include "Resource.hpp"

namespace game::world
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);

	static rendering::model::Mesh* _treeMesh = nullptr;
	static rendering::bounding_geometry::Sphere _treeBoundingGeometry = rendering::bounding_geometry::Sphere();

	static rendering::model::Mesh* getTreeMesh()
	{
		if (_treeMesh == nullptr)
			_treeMesh = new rendering::model::Mesh("tree", _treeBoundingGeometry);

		return _treeMesh;
	}

	Tree::Tree() : Resource(getTreeMesh(), &_treeBoundingGeometry) {}

	void Tree::addedToCell()
	{
		transform.setTransform(rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			2.0f * M_PI * randomFloat(generator), 0.0f, 0.0f,
			glm::vec3(1.0f)
		).toTransformationMatrix());
	}
}
