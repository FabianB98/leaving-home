#include "Resource.hpp"

namespace game::world
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);

	static std::shared_ptr<rendering::model::MeshData> treeMeshData = std::make_shared<rendering::model::MeshData>("tree");

	Tree::Tree() : Resource(treeMeshData) {}

	void Tree::addedToCell()
	{
		transform.setTransform(rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			2.0f * M_PI * randomFloat(generator), 0.0f, 0.0f,
			glm::vec3(1.0f)
		).toTransformationMatrix());
	}
}
