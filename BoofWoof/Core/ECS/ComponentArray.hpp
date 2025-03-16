#pragma once
#ifndef COMPONENT_ARRAY_H
#define COMPONENT_ARRAY_H

#include "pch.h"

// The one instance of virtual inheritance in the entire implementation.
// An interface is needed so that the ComponentManager (seen later)
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
	virtual void CopyComponent(Entity entity, Entity entityToCopy) = 0;
	virtual const size_t GetEntitySize() = 0;
};


template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void InsertData(Entity entity, T component)
	{
		// Ensure entity ID is in range
		assert(entity < MAX_ENTITIES && "Entity ID out of range");

		assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

		// Put new entry at end and update the maps
		size_t newIndex = mSize;
		mEntityToIndexMap[entity] = newIndex;
		mIndexToEntityMap[newIndex] = entity;
		mComponentArray[newIndex] = component;
		++mSize;
	}

	void RemoveData(Entity entity)
	{
		// Ensure entity ID is in range
		assert(entity < MAX_ENTITIES && "Entity ID out of range");
		auto it = mEntityToIndexMap.find(entity);
		assert(it != mEntityToIndexMap.end() && "Removing non-existent component.");

		size_t indexOfRemovedEntity = it->second;

		// Guard against empty array (should not happen if the map has an entry)
		if (mSize == 0)
		{
			std::cerr << "Component array is already empty when trying to remove entity " << entity << std::endl;
			return;
		}

		size_t indexOfLastElement = mSize - 1;

		// If the component being removed is not already the last element, copy the last element into its place.
		if (indexOfRemovedEntity != indexOfLastElement)
		{
			mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

			// Update the maps so the moved component’s entity now maps to indexOfRemovedEntity.
			Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
			mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;
		}

		mEntityToIndexMap.erase(entity);
		mIndexToEntityMap.erase(indexOfLastElement);
		--mSize;
	}


	T& GetData(Entity entity)
	{
		// Ensure entity ID is in range
		assert(entity < MAX_ENTITIES && "Entity ID out of range");

		assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

		// Return a reference to the entity's component
		return mComponentArray[mEntityToIndexMap[entity]];
	}

	void EntityDestroyed(Entity entity) override
	{
		if (entity < MAX_ENTITIES && mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			// Remove the entity's component if it existed
			RemoveData(entity);
		}
	}

	// Check if entity has component
	bool HaveComponent(Entity entity)
	{
		if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
		{
			return true;
		}
		return false;
	}

// Helper struct to check if T has SetComponentEntityID method
	template <typename U>
	class HasSetComponentEntityID
	{
	private:
		// Check if U has a method named SetComponentEntityID that takes an Entity as argument
		template <typename V>
		static auto Check(V*) -> decltype(std::declval<V>().SetComponentEntityID(std::declval<Entity>()), std::true_type());

		template <typename>
		static std::false_type Check(...);

	public:
		static constexpr bool value = decltype(Check<U>(nullptr))::value;
	};

	// Modified CopyComponent function
	void CopyComponent(Entity entity, Entity entityToCopy) override
	{
		T* copy_component = &mComponentArray[mEntityToIndexMap[entity]];
		T* clone_component = new T(*copy_component);

		// Conditionally call SetComponentEntityID if it exists
		if constexpr (HasSetComponentEntityID<T>::value)
		{
			clone_component->SetComponentEntityID(entityToCopy);
		}

		InsertData(entityToCopy, *clone_component);
		delete clone_component;
	}


	// getter functions
	std::array<size_t, MAX_ENTITIES>& GetEntityToIndexMap() {
		return mEntityToIndexMap;
	}

	std::array<Entity, MAX_ENTITIES>& GetIndexToEntityMap() {
		return mIndexToEntityMap;
	}

	// return size of entity
	const size_t GetEntitySize() {
		return mSize;
	}

private:
	// The packed array of components (of generic type T),
	// set to a specified maximum amount, matching the maximum number
	// of entities allowed to exist simultaneously, so that each entity
	// has a unique spot.
	std::array<T, MAX_ENTITIES> mComponentArray;

	// Map from an entity ID to an array index.
	std::unordered_map<Entity, size_t> mEntityToIndexMap;

	// Map from an array index to an entity ID.
	std::unordered_map<size_t, Entity> mIndexToEntityMap;

	// Total size of valid entries in the array.
	size_t mSize;
};

#endif