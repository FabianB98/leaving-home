#include "Resource.hpp"

namespace game::world
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);

	static std::shared_ptr<rendering::model::MeshData> treeMeshData = std::make_shared<rendering::model::MeshData>("tree");

	void Resource::addedToCell(Cell* cell)
	{
		setMeshDataAndTransform(cell, meshData, rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			0.0f, 0.0f, 0.0f,
			glm::vec3(1.0f)
		).toTransformationMatrix());

		_addedToCell(cell);
	}

	void Resource::removedFromCell(Cell* cell)
	{
		_removedFromCell(cell);
	}

	Tree::Tree() : Resource(treeMeshData) {}

	void Tree::_addedToCell(Cell* cell)
	{
		setTransform(cell, rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			2.0f * M_PI * randomFloat(generator), 0.0f, 0.0f,
			glm::vec3(1.0f)
		).toTransformationMatrix());
	}

	void Tree::_removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}
}
