#pragma once

#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H


#include "pch.h"

class EntityManager
{
public:
	EntityManager()
	{
		// Initialize the queue with all possible entity IDs
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			mAvailableEntities.push(entity);
		}
	}

	Entity CreateEntity()
	{
		assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

		// take an ID from the front of the queue and assign it to the entity
		Entity id = mAvailableEntities.top();
		// pop the ID from the queue
		mAvailableEntities.pop();
		// add to set of alive entities
		mAliveEntities.emplace_back(id);
		// increment the amount of living entities
		++mLivingEntityCount;

		return id;
	}

	void DestroyEntity(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Invalidate the destroyed entity's signature
		mSignatures[entity].reset();

		// Put the destroyed ID at the back of the queue
		mAvailableEntities.push(entity);

		auto it = std::find(mAliveEntities.begin(), mAliveEntities.end(), entity);
		if (it != mAliveEntities.end()) 
		{
			mAliveEntities.erase(it);
		}

		--mLivingEntityCount;
	}

	void SetSignature(Entity entity, Signature signature)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Put this entity's signature into the array
		mSignatures[entity] = signature;
	}

	Signature GetSignature(Entity entity)
	{
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Get this entity's signature from the array
		return mSignatures[entity];
	}

	Entity GetTotalEntites() 
	{
		return mLivingEntityCount;
	}

	const std::vector<Entity> GetAliveEntites() 
	{
		return mAliveEntities;
	}

	// gets entity signature array 
	std::array<Signature, MAX_ENTITIES>& GetEntitySignatureArray()
	{
		return mSignatures;
	}

	// Resets entity signature
	void ResetAllEntitySignatures() 
	{
		mSignatures.fill(0);
	}

	// Get entity ID
	Entity GetEntityId(Entity entity) 
	{
//		std::cout << "Current ENTITY:\t" << entity << '\n';

		// Ensure that the provided entity is within range
		assert(entity < MAX_ENTITIES && "Entity ID out of range");

		return entity;
	}

private:
	// use a queue for entity IDs
	std::priority_queue<Entity, std::vector<Entity>, std::greater<Entity>> mAvailableEntities{};

	// Array of signatures where the index corresponds to the entity ID
	std::array<Signature, MAX_ENTITIES> mSignatures{};

	// Total living entities - used to keep limits on how many exist
	Entity mLivingEntityCount{};

	// Set of alive entities
	std::vector<Entity> mAliveEntities{};
};

#endif