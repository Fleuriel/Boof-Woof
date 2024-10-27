#pragma once
#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "pch.h"
#include "ComponentArray.hpp"

class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		// Gets the component array
		auto compArray = std::make_shared<ComponentArray<T>>();

		assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

		// Add this component type to the component type map
		mComponentTypes.insert({ typeName, mNextComponentType });

		// Create a ComponentArray pointer and add it to the component arrays map
		mComponentArrays.insert({ typeName, compArray });

		ComponentType type = mComponentTypes[typeName];

		mComponentArrayFromType.try_emplace(type, compArray);

		// Increment the value so that the next component registered will be different
		++mNextComponentType;

		++mComponentsRegistered;
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		// Return this component's type - used for creating signatures
		return mComponentTypes[typeName];
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		// Add a component to the array for an entity
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template <typename T>
	void RemoveComponent(Entity entity) 
	{
		// Remove a component from the array for an entity
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		// Get a reference to a component from the array for an entity
		return GetComponentArray<T>()->GetData(entity);
	}

	void EntityDestroyed(Entity entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : mComponentArrays)
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}

	template <typename T>
	bool HaveComponent(Entity entity) {
		return GetComponentArray<T>()->HaveComponent(entity);
	}

	const ComponentType GetTotalRegisteredComponents() {
		return mComponentsRegistered;
	}

	const size_t GetEntitySize(ComponentType type) {
		return GetComponentArrayFromType(type)->GetEntitySize();
	}

	inline std::shared_ptr<IComponentArray> GetComponentArrayFromType(ComponentType type) {
		return mComponentArrayFromType[type];
	}

private:
	// Static counter for number of components registered
	inline static ComponentType mComponentsRegistered{};

	// Map from type string pointer to a component type
	std::unordered_map<const char*, ComponentType> mComponentTypes{};

	// Map from type string pointer to a component array
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	// Map from component type to component array
	std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> mComponentArrayFromType{};

	// The component type to be assigned to the next registered component - starting at 0
	ComponentType mNextComponentType{};

	// Convenience function to get the statically casted pointer to the ComponentArray of type T.
	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}
};

#endif