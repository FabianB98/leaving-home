#pragma once

#include <functional>
#include <sstream>
#include <string>
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
		const std::string& typeName;

		IItem(float _amount, const std::string& _typeName) : amount(_amount), typeName(_typeName) {}

		virtual std::shared_ptr<IItem> clone() = 0;

		virtual std::shared_ptr<IItem> clone(float clonedAmount) = 0;

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

		Inventory() {}

		Inventory(const std::unordered_set<std::shared_ptr<IItem>, IItemHash, IItemComparator>& _items) : items(_items) {}

		void addItems(const Inventory& source)
		{
			for (std::shared_ptr<IItem> item : source.items)
				addItem(item->clone());
		}

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

		float getStoredAmount(std::shared_ptr<IItem> itemType)
		{
			auto& found = items.find(itemType);
			if (found == items.end())
				return 0.0f;
			else
				return (*found)->amount;
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
			return std::dynamic_pointer_cast<T>(getItem(std::make_shared<T>(0.0f)));
		}

		std::shared_ptr<IItem> removeItem(std::shared_ptr<IItem> itemType, float maxAmount)
		{
			auto& found = items.find(itemType);
			if (found == items.end())
			{
				return nullptr;
			}
			else
			{
				std::shared_ptr<IItem> result = (*found)->split(maxAmount);

				if ((*found)->amount == 0.0f)
					items.erase(found);

				return result;
			}
		}

		template <class T>
		std::shared_ptr<T> removeItemTyped(float maxAmount)
		{
			return std::dynamic_pointer_cast<T>(removeItem(std::make_shared<T>(0.0f), maxAmount));
		}

		void split(unsigned int amount)
		{
			float multiplier = 1.0f / ((float) amount);

			for (std::shared_ptr<IItem> item : items)
				item->amount *= multiplier;
		}

		std::string getStoredItemsString()
		{
			std::stringstream storedItemsString;

			auto iterator = items.begin();
			while (iterator != items.end()) {
				const std::shared_ptr<IItem>& item = *iterator;

				storedItemsString << item->typeName << ": " << item->amount;
				if ((++iterator) != items.end())
					storedItemsString << std::endl;
			}

			return storedItemsString.str();
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
