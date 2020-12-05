#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace rendering::components
{
	struct Relationship
	{
		//Relationship() : parent(entt::null), level(0), totalTransform(glm::mat4(1)) {}
		//Relationship(entt::entity _parent) : parent(_parent), level(1), totalTransform(glm::mat4(1)) {}
		//Relationship() : parent(entt::null), level(0), totalTransform(glm::mat4(1)), children(0), first(entt::null), prev(entt::null), next(entt::null) {}

		// parent entity
		entt::entity parent{ entt::null };
		// distance from the root of this hierarchy
		unsigned int level{};

		glm::mat4 totalTransform{};

		std::size_t children{};
		entt::entity first{ entt::null };
		entt::entity prev{ entt::null };
		entt::entity next{ entt::null };
	};
}