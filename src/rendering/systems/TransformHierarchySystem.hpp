#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <execution>
#include <set>
#include <vector>
#include <unordered_map>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../components/Transform.hpp"
#include "../components/Relationship.hpp"

namespace rendering::systems
{
	void relationship(entt::registry& registry, entt::entity parent, entt::entity child);

	void initHierarchySystem(entt::registry& registry);

	void updateHierarchy(entt::registry& registry);

	void cleanUpHierarchySystem(entt::registry& registry);
}