#pragma once
#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H


#include "System.hpp"

class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

		// Create a pointer to the system and return it so it can be used externally
		auto system = std::make_shared<T>();
		mSystems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

		// Set the signature for this system
		mSignatures.insert({ typeName, signature });
	}
	
	void EntityDestroyed(Entity entity)
	{
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (auto const& pair : mSystems)
		{
			auto const& system = pair.second;

			// find and remove the entity from the system's entity list with safety checks
			auto it = std::find(system->mEntities.begin(), system->mEntities.end(), entity);
			if (it != system->mEntities.end()) {
				system->mEntities.erase(it);
			}
		}
	}

	void EntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (auto const& pair : mSystems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = mSignatures[type];

			bool entityPresent = false;

			// Check if entity is already present in system's entities
			for (const auto& existingEntity : system->mEntities) 
			{
				if (existingEntity == entity) 
				{
					entityPresent = true;
					break;
				}
			}

			// Entity signature matches system signature - insert into vector
			if ((entitySignature & systemSignature) == systemSignature)
			{
				if (!entityPresent) 
				{
					system->mEntities.emplace_back(entity);
				}
			}
			// Entity signature does not match system signature - erase from vector
			else
			{
				// Remove the entity if present
				if (entityPresent) 
				{
					auto it = std::find(system->mEntities.begin(), system->mEntities.end(), entity);
					if (it != system->mEntities.end()) 
					{
						system->mEntities.erase(it);
					}
				}
			}
		}
	}

	// Getter for the 'm_Signatures' map
	const std::unordered_map<const char*, Signature>& GetSignatures() const 
	{
		return mSignatures;
	}

	// Getter for the 'systems' map from SystemManager
	const std::unordered_map<const char*, std::shared_ptr<System>>& GetSystems() const 
	{
		return mSystems;
	}

	// clear systems
	void ClearSystem() { mSystems.clear(); }

	size_t GetSystemAlive() { return mSystems.size(); }

	template<typename T>
	std::shared_ptr<T> GetSystem()
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "System not registered.");

		return std::static_pointer_cast<T>(mSystems[typeName]);
	}


private:
	// Map from system type string pointer to a signature
	std::unordered_map<const char*, Signature> mSignatures{};

	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};

#endif