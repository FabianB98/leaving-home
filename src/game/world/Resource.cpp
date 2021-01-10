#include "Resource.hpp"

namespace game::world
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);

	static std::shared_ptr<rendering::model::MeshData> treeMeshData = std::make_shared<rendering::model::MeshData>("tree");
	static std::shared_ptr<rendering::model::MeshData> stoneMeshData = std::make_shared<rendering::model::MeshData>("stone");

	void Resource::_addedToCell(Cell* cell)
	{
		setMeshDataAndTransform(cell, meshData, rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			0.0f, 0.0f, 0.0f,
			glm::vec3(1.0f)
		).toTransformationMatrix());

		__addedToCell(cell);
	}

	void Resource::_removedFromCell(Cell* cell)
	{
		__removedFromCell(cell);
	}

	Tree::Tree() : Resource(treeMeshData) {}

	void Tree::__addedToCell(Cell* cell)
	{
		setTransform(cell, rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			2.0f * M_PI * randomFloat(generator), 0.0f, 0.0f,
			glm::vec3(1.0f)
		).toTransformationMatrix());

		entt::registry* registry = getRegistry();
		entt::entity& entity = getEntity();

		registry->get<Inventory>(entity).addItemTyped<Wood>(1.0f);
		registry->emplace<Harvestable<Wood>>(entity);
	}

	void Tree::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void Tree::update()
	{
		// Nothing to do here...
	}


	Rock::Rock() : Resource(stoneMeshData) {}

	void Rock::__addedToCell(Cell* cell)
	{
		auto transform = rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			2.0f * M_PI * randomFloat(generator), 0.0f, 0.0f,
			glm::vec3(0.75f + 0.25f * randomFloat(generator))
		).toTransformationMatrix();
		setTransform(cell, transform);

		//getRegistry()->emplace<Wood>(getEntity(), 1.0f);
		//getRegistry()->emplace<Harvestable<Wood>>(getEntity());
		getRegistry()->emplace<rendering::components::MatrixTransform>(getEntity(), transform);
		getRegistry()->emplace<rendering::components::PointLight>(getEntity(), glm::vec3(1.5,1.5,3), glm::vec3(0,1,0), glm::vec3(0,1,0.1));
	}

	void Rock::__removedFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void Rock::update()
	{
		// Nothing to do here...
	}
}
