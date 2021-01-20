#pragma once

#include <unordered_set>

#include "../systems/ResourceProcessingSystem.hpp"
#include "Inventory.hpp"

namespace game::world
{
	template <class T>
	struct Harvestable;

	template <class T>
	struct Stores;

	template <class T>
	struct Produces;

	template <class T>
	struct Consumes;

	template<typename Base, typename Derived>
	void _iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, Base*)>& func)
	{
		auto view = registry.view<Derived>();
		for (auto entity : view)
		{
			auto& item = view.get<Derived>(entity);
			func(entity, &item);
		}
	};

	template<typename T>
	entt::entity _getAny(entt::registry& registry)
	{
		auto view = registry.view<T>();
		for (auto entity : view)
			return entity;

		return entt::null;
	}

	template <class T>
	struct Item : public IItem
	{
	public:
		Item(const std::string& _typeName) : IItem(0.0f, _typeName) {}

		Item(float _amount, const std::string& _typeName) : IItem(_amount, _typeName) 
		{
			systems::registerItemType(typeRepresentative);
		}

		std::shared_ptr<IItem> clone()
		{
			return std::make_shared<T>(amount);
		}

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

		IHarvestable* getHarvestable()
		{
			return &harvestable;
		}

		IStores* getStores()
		{
			return &stores;
		}

		IProduces* getProduces()
		{
			return &produces;
		}

		IConsumes* getConsumes()
		{
			return &consumes;
		}

		size_t getTypeHashCode() const
		{
			return typeid(T).hash_code();
		}

		std::type_index getTypeIndex() const
		{
			return std::type_index(typeid(T));
		}

	private:
		static std::shared_ptr<T> typeRepresentative;
		static Harvestable<T> harvestable;
		static Stores<T> stores;
		static Produces<T> produces;
		static Consumes<T> consumes;
	};

	template <class T>
	std::shared_ptr<T> Item<T>::typeRepresentative = std::make_shared<T>();

	template <class T>
	Harvestable<T> Item<T>::harvestable = Harvestable<T>();

	template <class T>
	Stores<T> Item<T>::stores = Stores<T>();

	template <class T>
	Produces<T> Item<T>::produces = Produces<T>();

	template <class T>
	Consumes<T> Item<T>::consumes = Consumes<T>();

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

		entt::entity getAny(entt::registry& registry)
		{
			return _getAny<Harvestable<T>>(registry);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IHarvestable*)>& func)
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

		entt::entity getAny(entt::registry& registry)
		{
			return _getAny<Stores<T>>(registry);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IStores*)>& func)
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

		entt::entity getAny(entt::registry& registry)
		{
			return _getAny<Produces<T>>(registry);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IProduces*)>& func)
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

		entt::entity getAny(entt::registry& registry)
		{
			return _getAny<Consumes<T>>(registry);
		}

		void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IConsumes*)>& func)
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
		static const std::string TYPE_NAME;

		Wood() : Item(TYPE_NAME) {}

		Wood(float _amount) : Item(_amount, TYPE_NAME) {}
	};
}
