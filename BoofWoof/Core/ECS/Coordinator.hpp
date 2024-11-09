#pragma once
#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "pch.h"
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"

#define g_Coordinator Coordinator::GetInstance()

class Coordinator
{
public:

	// get instance of ECS being used
	static Coordinator& GetInstance() 
	{
		static Coordinator instance{};
		return instance;
	}

	void Init()
	{
		// Create pointers to each manager
		mComponentManager = std::make_unique<ComponentManager>();
		mEntityManager = std::make_unique<EntityManager>();
		mSystemManager = std::make_unique<SystemManager>();

		std::cout << "Coordinator Initialized!\n";
	}

	EntityManager& GetEntityManager() {
		return *mEntityManager;
	}

	ComponentManager& GetComponentManager() {
		return *mComponentManager;
	}

	// Entity methods
	Entity CreateEntity()
	{
		return mEntityManager->CreateEntity();
	}

	void DestroyEntity(Entity entity)
	{
		mEntityManager->DestroyEntity(entity);

		mComponentManager->EntityDestroyed(entity);

		mSystemManager->EntityDestroyed(entity);
	}

	// Clone Entity
	Entity CloneEntity(Entity entity) 
	{
		Entity new_entity = mEntityManager->CreateEntity();
		mEntityManager->SetSignature(new_entity, GetEntitySignature(entity));
		for (ComponentType i{ 0 }; i < GetTotalRegisteredComponents(); ++i) {
			if (GetEntitySignature(new_entity).test(i)) {
				mComponentManager->GetComponentArrayFromType(i)->CopyComponent(entity, new_entity);
			}
		}
		auto signature = mEntityManager->GetSignature(new_entity);
		mSystemManager->EntitySignatureChanged(new_entity, signature);

		return new_entity;
	}

	Entity GetTotalEntities() 
	{
		return mEntityManager->GetTotalEntites();
	}

	// Gets the number of alive entities
	Entity GetAliveEntities() 
	{
		return static_cast<Entity>(mEntityManager->GetAliveEntites().size());
	}

	// Gets the set of entites alive
	std::vector<Entity> GetAliveEntitiesSet() 
	{
		return mEntityManager->GetAliveEntites();
	}

	Entity GetEntityId(Entity entity) 
	{
		return mEntityManager->GetEntityId(entity);
	}

	void ResetEntities()
	{
		auto alive_set = mEntityManager->GetAliveEntites();

		for (const auto& it : alive_set) {
			DestroyEntity(it);
		}
	}

	void ResetAllEntitySignatures() 
	{
		mEntityManager->ResetAllEntitySignatures();
	}

	Signature GetEntitySignature(Entity entity)
	{
		return mEntityManager->GetSignature(entity);
	}

	// set entity signature to clone
	void SetEntitySignature(Entity entity, Signature signature)
	{
		mEntityManager->SetSignature(entity, signature);
	}

	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		mComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component)
	{
		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		mComponentManager->RemoveComponent<T>(entity);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), false);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		return mComponentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return mComponentManager->GetComponentType<T>();
	}

	template <typename T>
	bool HaveComponent(Entity entity) {
		return mComponentManager->HaveComponent<T>(entity);
	}


	const ComponentType GetTotalRegisteredComponents() {
		return mComponentManager->GetTotalRegisteredComponents();
	}

	const size_t GetEntitySize(ComponentType type) {
		return mComponentManager->GetEntitySize(type);
	}

	std::shared_ptr<IComponentArray> GetComponentArrayFromType(ComponentType type) {
		return mComponentManager->GetComponentArrayFromType(type);
	}

	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return mSystemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature signature)
	{
		mSystemManager->SetSignature<T>(signature);
	}

	// Get signatures mapped from System Manager
	const std::unordered_map<const char*, Signature>& GetSystemSignatures() const {
		return mSystemManager->GetSignatures();
	}

	// get systems map from system manager
	const std::unordered_map<const char*, std::shared_ptr<System>>& GetSystems() const {
		return mSystemManager->GetSystems();
	}

	void ClearSystems() {
		mSystemManager->ClearSystem();
	}

	size_t GetSystemAlive() {
		return mSystemManager->GetSystemAlive();
	}

	template<typename T>
	std::shared_ptr<T> GetSystem()
	{
		return mSystemManager->GetSystem<T>();
	}


private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;
};

#endif