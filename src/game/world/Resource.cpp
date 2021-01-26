#include "Resource.hpp"

namespace game::world
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);

	static const Inventory emptyInventory = Inventory();

	static const std::string treeTypeName = "Tree";
	static const std::string treeDescription = "A tree is a reliable source of wood. What else did you expect?";
	static std::shared_ptr<rendering::model::MeshData> treeMeshData = std::make_shared<rendering::model::MeshData>("tree");
	static std::shared_ptr<rendering::model::MeshData> treeMeshData2 = std::make_shared<rendering::model::MeshData>("tree2");

	static const std::string rockTypeName = "Rock";
	static const std::string rockDescription = "A glowing rock. May contain precious ores.";
	static std::shared_ptr<rendering::model::MeshData> stoneMeshData = std::make_shared<rendering::model::MeshData>("stone");

	void Resource::_enqueuedToAddToCell(Cell* cell)
	{
		// Nothing to do here (at least as long as resources can't be placed by the user)...
	}

	void Resource::_addedToCell(Cell* cell)
	{
		setMeshDataAndTransform(cell, meshData, rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			0.0f, 0.0f, 0.0f,
			glm::vec3(1.0f)
		).toTransformationMatrix());

		__addedToCell(cell);
	}

	void Resource::_enqueuedToRemoveFromCell(Cell* cell)
	{
		// Nothing to do here...
	}

	void Resource::_removedFromCell(Cell* cell)
	{
		__removedFromCell(cell);
	}

	void Resource::inventoryUpdated()
	{
		// If the resource's inventory is empty, the resource was fully harvested, so it needs to be removed from the world.
		Inventory& inventory = getRegistry()->get<Inventory>(getEntity());
		if (inventory.items.empty())
		{
			// Copying the cells is needed because the removal will (a) modify the cells map which would cause an iteration over
			// a changed map and (b) eventually delete this object which could cause null pointer exceptions and illegal memory
			// accesses.
			std::vector<Cell*> cellsToRemoveFrom;
			for (auto& cell : getCells())
				cellsToRemoveFrom.push_back(cell.first);

			for (Cell* cell : cellsToRemoveFrom)
				cell->setContent(nullptr);
		}
	}

	const Inventory Resource::_getResourcesObtainedByRemoval(Cell* cell)
	{
		return emptyInventory;
	}

	Tree::Tree() : Resource(treeTypeName, treeDescription, randomFloat(generator) > 0.5f ? treeMeshData : treeMeshData2) {}

	void Tree::__addedToCell(Cell* cell)
	{
		setTransform(cell, rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			2.0f * M_PI * randomFloat(generator), 0.0f, 0.0f,
			glm::vec3(0.8f + 0.3f * randomFloat(generator))
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


	Rock::Rock() : Resource(rockTypeName, rockDescription, stoneMeshData) {}

	void Rock::__addedToCell(Cell* cell)
	{
		auto transform = rendering::components::EulerComponentwiseTransform(
			cell->getRelaxedPositionAndHeight(),
			2.0f * M_PI * randomFloat(generator), 0.0f, 0.0f,
			glm::vec3(0.75f + 0.25f * randomFloat(generator))
		).toTransformationMatrix();
		setTransform(cell, transform);

		entt::registry* registry = getRegistry();
		entt::entity& entity = getEntity();

		registry->get<Inventory>(entity).addItemTyped<Stone>(1.0f);
		registry->emplace<Harvestable<Stone>>(entity);

		registry->get<Inventory>(entity).addItemTyped<Ores>(0.5f);
		registry->emplace<Harvestable<Ores>>(entity);

		registry->emplace<rendering::components::MatrixTransform>(entity, transform);
		registry->emplace<rendering::components::PointLight>(entity, glm::vec3(1.5,1.5,3), glm::vec3(0,1,0), glm::vec3(0,1,0.1));
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
