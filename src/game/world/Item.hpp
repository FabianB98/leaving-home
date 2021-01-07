#pragma once

#include <functional>

#include <entt/entt.hpp>

namespace game::world
{
	struct IHarvestable;
	struct IStores;
	struct IProduces;
	struct IConsumes;

	struct IItem
	{
		float amount;

		IItem(float _amount) : amount(_amount) {}

		virtual IItem* split(float amountToSplit) = 0;

		virtual IItem* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IItem*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual IHarvestable* getHarvestable() = 0;

		virtual IStores* getStores() = 0;

		virtual IProduces* getProduces() = 0;

		virtual IConsumes* getConsumes() = 0;
	};

	struct IHarvestable 
	{
		virtual IItem* getItem() = 0;

		virtual IHarvestable* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IHarvestable*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IStores
	{
		virtual IItem* getItem() = 0;

		virtual IStores* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IStores*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IProduces
	{
		virtual IItem* getItem() = 0;

		virtual IProduces* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IProduces*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IConsumes
	{
		virtual IItem* getItem() = 0;

		virtual IConsumes* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IConsumes*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	template <class T>
	struct Harvestable;

	template <class T>
	struct Stores;

	template <class T>
	struct Produces;

	template <class T>
	struct Consumes;

	template <class T>
	struct Item : public IItem
	{
		Item(float _amount) : IItem(_amount) {}

		IItem* split(float amountToSplit)
		{
			float resultAmount;
			if (amountToSplit >= amount)
			{
				amount -= amountToSplit;
				resultAmount = amountToSplit;
			}
			else
			{
				resultAmount = amount;
				amount = 0.0f;
			}

			return new T(resultAmount);
		}

		IItem* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<T>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IItem*)> func)
		{
			auto view = registry.view<T>();
			for (auto entity : view)
			{
				auto& item = view.get<T>(entity);
				func(entity, &item);
			}
		}

		void addToEntity(entt::registry& registry, entt::entity& entity)
		{
			T* item = registry.try_get<T>(entity);
			if (item == nullptr)
				registry.emplace<T>(entity, amount);
			else
				item->amount += amount;
		}

		void removeFromEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.remove_if_exists<T>(entity);
		}

		IHarvestable* getHarvestable()
		{
			return new Harvestable<T>();
		}

		IStores* getStores()
		{
			return new Stores<T>();
		}

		IProduces* getProduces()
		{
			return new Produces<T>();
		}

		IConsumes* getConsumes()
		{
			return new Consumes<T>();
		}
	};

	template <class T>
	struct Harvestable : public IHarvestable
	{
		IItem* getItem()
		{
			return new T(0.0f);
		}

		IHarvestable* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Harvestable<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IHarvestable*)> func)
		{
			auto view = registry.view<Harvestable<T>>();
			for (auto entity : view)
			{
				auto& item = view.get<Harvestable<T>>(entity);
				func(entity, &item);
			}
		}

		void addToEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.emplace<Harvestable<T>>(entity);
		}

		void removeFromEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.remove_if_exists<Harvestable<T>>(entity);
		}
	};

	template <class T>
	struct Stores : public IStores
	{
		IItem* getItem()
		{
			return new T(0.0f);
		}

		IStores* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Stores<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IStores*)> func)
		{
			auto view = registry.view<Stores<T>>();
			for (auto entity : view)
			{
				auto& item = view.get<Stores<T>>(entity);
				func(entity, &item);
			}
		}

		void addToEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.emplace<Stores<T>>(entity);
		}

		void removeFromEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.remove_if_exists<Stores<T>>(entity);
		}
	};

	template <class T>
	struct Produces : public IProduces
	{
		IItem* getItem()
		{
			return new T(0.0f);
		}

		IProduces* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Produces<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IProduces*)> func)
		{
			auto view = registry.view<Produces<T>>();
			for (auto entity : view)
			{
				auto& item = view.get<Produces<T>>(entity);
				func(entity, &item);
			}
		}

		void addToEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.emplace<Produces<T>>(entity);
		}

		void removeFromEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.remove_if_exists<Produces<T>>(entity);
		}
	};

	template <class T>
	struct Consumes : public IConsumes
	{
		IItem* getItem()
		{
			return new T(0.0f);
		}

		IConsumes* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Consumes<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IConsumes*)> func)
		{
			auto view = registry.view<Consumes<T>>();
			for (auto entity : view)
			{
				auto& item = view.get<Consumes<T>>(entity);
				func(entity, &item);
			}
		}

		void addToEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.emplace<Consumes<T>>(entity);
		}

		void removeFromEntity(entt::registry& registry, entt::entity& entity)
		{
			registry.remove_if_exists<Consumes<T>>(entity);
		}
	};

	struct Wood : public Item<Wood> 
	{
		Wood(float _amount) : Item(_amount) {}
	};
}
