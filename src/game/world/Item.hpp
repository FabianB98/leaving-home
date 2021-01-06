#pragma once

#include <entt/entt.hpp>

namespace game::world
{
	struct Item
	{
		float amount;
	};

	template <class T>
	struct Harvestable {};

	template <class T>
	struct Stored {};

	template <class T>
	struct Produces {};

	template <class T>
	struct Consumes {};

	struct Wood : public Item {};
}
