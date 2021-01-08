#pragma once

#include <functional>
#include <typeindex>
#include <unordered_set>

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

		virtual std::shared_ptr<IItem> split(float amountToSplit) = 0;

		virtual std::shared_ptr<IHarvestable> getHarvestable() = 0;

		virtual std::shared_ptr<IStores> getStores() = 0;

		virtual std::shared_ptr<IProduces> getProduces() = 0;

		virtual std::shared_ptr<IConsumes> getConsumes() = 0;

		virtual size_t getTypeHashCode() const = 0;

		virtual std::type_index getTypeIndex() const = 0;
	};

	struct IItemHash
	{
		size_t operator() (const std::shared_ptr<IItem> item) const
		{
			return item->getTypeHashCode();
		}
	};

	struct IItemComparator
	{
		bool operator() (const std::shared_ptr<IItem> a, const std::shared_ptr<IItem> b) const
		{
			return a->getTypeIndex() == b->getTypeIndex();
		}
	};

	struct Inventory
	{
		std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator> items;

		void addItem(std::shared_ptr<IItem> item)
		{
			if (item == nullptr)
				return;

			auto& found = items.find(item);
			if (found == items.end())
				items.insert(item);
			else
				(*found)->amount += item->amount;
		}

		template <class T>
		void addItemTyped(float amount)
		{
			addItem(std::make_shared<T>(amount));
		}

		template <class T>
		std::shared_ptr<T> getItem()
		{
			auto& found = items.find(std::make_shared<T>(0.0f));
			if (found == items.end())
				return nullptr;
			else
				return std::dynamic_pointer_cast<T>(*found);
		}

		template <class T>
		std::shared_ptr<T> removeItem(float maxAmount)
		{
			auto& found = items.find(std::make_shared<T>(0.0f));
			if (found == items.end())
				return nullptr;
			else
				return std::dynamic_pointer_cast<T>((*found)->split(maxAmount));
		}
	};

	struct IHarvestable 
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

		virtual IHarvestable* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IHarvestable*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IStores
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

		virtual IStores* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IStores*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IProduces
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

		virtual IProduces* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IProduces*)> func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IConsumes
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

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

	template<typename Base, typename Derived>
	void _iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, Base*)> func)
	{
		auto view = registry.view<Derived>();
		for (auto entity : view)
		{
			auto& item = view.get<Derived>(entity);
			func(entity, &item);
		}
	};

	template <class T>
	struct Item : public IItem
	{
		Item(float _amount) : IItem(_amount) {}

		std::shared_ptr<IItem> split(float amountToSplit)
		{
			float resultAmount;
			if (amountToSplit < 0.0f || amountToSplit > amount)
			{
				resultAmount = amount;
				amount = 0.0f;
			}
			else
			{
				amount -= amountToSplit;
				resultAmount = amountToSplit;
			}

			return std::make_shared<T>(resultAmount);
		}

		std::shared_ptr<IHarvestable> getHarvestable()
		{
			return std::make_shared<Harvestable<T>>();
		}

		std::shared_ptr<IStores> getStores()
		{
			return std::make_shared<Stores<T>>();
		}

		std::shared_ptr<IProduces> getProduces()
		{
			return std::make_shared<Produces<T>>();
		}

		std::shared_ptr<IConsumes> getConsumes()
		{
			return std::make_shared<Consumes<T>>();
		}

		std::type_info getTypeid() const
		{
			return typeid(T);
		}

		size_t getTypeHashCode() const
		{
			return typeid(T).hash_code();
		}

		std::type_index getTypeIndex() const
		{
			return std::type_index(typeid(T));
		}
	};

	template <class T>
	struct Harvestable : public IHarvestable
	{
		std::shared_ptr<IItem> getItem()
		{
			return std::make_shared<T>(0.0f);
		}

		IHarvestable* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Harvestable<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IHarvestable*)> func)
		{
			_iterateAllEntities<IHarvestable, Harvestable<T>>(registry, func);
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
		std::shared_ptr<IItem> getItem()
		{
			return std::make_shared<T>(0.0f);
		}

		IStores* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Stores<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IStores*)> func)
		{
			_iterateAllEntities<IStores, Stores<T>>(registry, func);
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
		std::shared_ptr<IItem> getItem()
		{
			return std::make_shared<T>(0.0f);
		}

		IProduces* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Produces<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IProduces*)> func)
		{
			_iterateAllEntities<IProduces, Produces<T>>(registry, func);
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
		std::shared_ptr<IItem> getItem()
		{
			return std::make_shared<T>(0.0f);
		}

		IConsumes* getFromEntity(entt::registry& registry, entt::entity& entity)
		{
			return registry.try_get<Consumes<T>>(entity);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IConsumes*)> func)
		{
			_iterateAllEntities<IConsumes, Consumes<T>>(registry, func);
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
