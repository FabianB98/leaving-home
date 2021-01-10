#pragma once

#include <functional>
#include <typeindex>

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

		virtual IHarvestable* getHarvestable() = 0;

		virtual IStores* getStores() = 0;

		virtual IProduces* getProduces() = 0;

		virtual IConsumes* getConsumes() = 0;

		virtual size_t getTypeHashCode() const = 0;

		virtual std::type_index getTypeIndex() const = 0;
	};

	struct IItemHash
	{
		size_t operator() (const IItem* item) const
		{
			return item->getTypeHashCode();
		}

		size_t operator() (const std::shared_ptr<IItem> item) const
		{
			return item->getTypeHashCode();
		}
	};

	struct IItemComparator
	{
		bool operator() (const IItem* a, const IItem* b) const
		{
			return a->getTypeIndex() == b->getTypeIndex();
		}

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

		std::shared_ptr<IItem> getItem(std::shared_ptr<IItem> itemType)
		{
			auto& found = items.find(itemType);
			if (found == items.end())
				return nullptr;
			else
				return *found;
		}

		template <class T>
		std::shared_ptr<T> getItemTyped()
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
			{
				return nullptr;
			}
			else
			{
				std::shared_ptr<T> result = std::dynamic_pointer_cast<T>((*found)->split(maxAmount));

				if ((*found)->amount == 0.0f)
					items.erase(found);

				return result;
			}
		}
	};

	struct IHarvestable
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

		virtual IHarvestable* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual entt::entity getAny(entt::registry& registry) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IHarvestable*)>& func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IStores
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

		virtual IStores* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual entt::entity getAny(entt::registry& registry) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IStores*)>& func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IProduces
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

		virtual IProduces* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual entt::entity getAny(entt::registry& registry) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IProduces*)>& func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};

	struct IConsumes
	{
		virtual std::shared_ptr<IItem> getItem() = 0;

		virtual IConsumes* getFromEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual entt::entity getAny(entt::registry& registry) = 0;

		virtual void iterateAllEntities(entt::registry& registry, std::function<void(entt::entity&, IConsumes*)>& func) = 0;

		virtual void addToEntity(entt::registry& registry, entt::entity& entity) = 0;

		virtual void removeFromEntity(entt::registry& registry, entt::entity& entity) = 0;
	};
}
