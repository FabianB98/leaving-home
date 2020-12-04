#include "TransformHierarchySystem.hpp"

namespace rendering::systems
{
	entt::observer transformRelationshipObserver;
	std::vector<entt::entity> changed;
	std::set<entt::entity> newParents;

	void relationship(entt::registry& registry, entt::entity parent, entt::entity child)
	{
		auto& pRel = registry.get<components::Relationship>(parent);
		auto& cRel = registry.get<components::Relationship>(child);

		cRel.parent = parent;
		cRel.level = pRel.level + 1;

		// this is the first child of parent
		if (pRel.first == entt::null) {
			pRel.first = child;
			cRel.next = cRel.prev = child;
		}
		else {
			// insert child as the first item in the double linked list
			auto first = pRel.first;
			auto& firstRel = registry.get<components::Relationship>(pRel.first);
			pRel.first = child;
			cRel.prev = firstRel.prev;
			cRel.next = first;
			firstRel.prev = child;
		}
		pRel.children++;

		if (newParents.find(parent) == newParents.end())
			newParents.insert(parent);
	}

	void initHierarchySystem(entt::registry& registry)
	{
		transformRelationshipObserver.connect(registry, 
			entt::collector.update<components::MatrixTransform>().where<components::Relationship>());
	}

	void updateFromParent(entt::registry& registry, entt::entity entity,
		components::Relationship& relationship,
		components::MatrixTransform& transform)
	{
		// check if this entity is a root or not
		if (relationship.parent == entt::null || relationship.level == 0) {
			relationship.totalTransform = transform.getTransform();
		}
		else {
			auto& parent = registry.get<components::Relationship>(relationship.parent);
			relationship.totalTransform = parent.totalTransform * transform.getTransform();
		}
		registry.replace<components::MatrixTransform>(entity, transform.getTransform());
	}



	void iterateChildren(entt::registry& registry, entt::entity entity,
		components::Relationship& relationship,
		std::vector<entt::entity>* dirty)
	{
		auto curr = relationship.first;

		for (std::size_t i{}; i < relationship.children; ++i) {
			auto& child = registry.get<components::Relationship>(curr);
			auto& transform = registry.get<components::MatrixTransform>(curr);
			child.totalTransform = relationship.totalTransform * transform.getTransform();
			registry.replace<components::MatrixTransform>(curr, transform.getTransform());

			// if this child has children itself, add it to the dirty-list to be processed next
			if (child.children > 0) {
				dirty->push_back(curr);
			}

			curr = child.next;
		}
	}

	auto* bfs = new std::vector<entt::entity>();
	auto* bfsSwap = new std::vector<entt::entity>();

	void updateHierarchy(entt::registry& registry)
	{
		// Get all entities whose transform was changed and store their level.
		for (const auto entity : transformRelationshipObserver) {
			auto& component = registry.get<components::Relationship>(entity);
			changed.push_back(entity);
		}
		// add new relationships to changed entities
		changed.insert(changed.end(), newParents.begin(), newParents.end());
		newParents.clear();

		// sort changed transforms so that parents are always in front of their children
		// see https://skypjack.github.io/2019-08-20-ecs-baf-part-4-insights/
		std::sort(changed.begin(), changed.end(), [&registry](auto e1, auto e2) {
			const auto& r1 = registry.get<components::Relationship>(e1);
			const auto& r2 = registry.get<components::Relationship>(e2);
			return r2.parent == e1 || r1.next == e2
				|| (!(r1.parent == e2 || r2.next == e1) && (r1.parent < r2.parent || (r1.parent == r2.parent && &r1 < &r2)));
		});

		for (auto entity : changed) {
			//auto entity = pair.first;
			auto& comp = registry.get<components::Relationship>(entity);
			auto& tf = registry.get<components::MatrixTransform>(entity);
			updateFromParent(registry, entity, comp, tf);
			iterateChildren(registry, entity, comp, bfs);
		}

		while (bfs->size() > 0) {
			for (auto entity : *bfs) {
				auto& comp = registry.get<components::Relationship>(entity);
				iterateChildren(registry, entity, comp, bfsSwap);
			}
			bfs->clear();
			std::swap(bfs, bfsSwap);
		}

		changed.clear();
		transformRelationshipObserver.clear();
	}

	void cleanUpHierarchySystem(entt::registry& registry)
	{
		transformRelationshipObserver.disconnect();
	}
}