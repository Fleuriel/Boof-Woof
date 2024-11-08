#pragma once

#include "pch.h"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "../../Utilities/Components/TransformComponent.hpp"

#define g_Arch ArchetypeManager::GetInstance()

class Archetype 
{
public:
    std::string name; // Name of the archetype
    std::vector<std::type_index> componentTypes; // Store component types

    Archetype(const std::string& archetypeName, const std::vector<std::type_index>& types) : name(archetypeName), componentTypes(types) {}
};

class ArchetypeManager {
public:

    // get instance of ECS being used
    static ArchetypeManager& GetInstance()
    {
        static ArchetypeManager instance{};
        return instance;
    }

    Archetype* createArchetype(const std::string& archetypeName, const std::vector<std::type_index>& componentTypes)
    {
        archetypes.emplace_back(archetypeName, componentTypes);
        return &archetypes.back(); // Return the newly created archetype
    }

    Entity createEntity(Archetype* archetype)
    {
        // Access the EntityManager and ComponentManager
        EntityManager& entityManager = g_Coordinator.GetEntityManager();
        ComponentManager& componentManager = g_Coordinator.GetComponentManager();

        Entity entity = entityManager.CreateEntity();

        // Set the entity's signature based on the archetype
        Signature signature = createSignature(archetype);
        entityManager.SetSignature(entity, signature);

        // Initialize components for the new entity based on the archetype
        for (const auto& type : archetype->componentTypes) 
        {
            if (type == typeid(TransformComponent))
            {
                // Check if the entity has the component of this type
                if (!componentManager.HaveComponent<TransformComponent>(entity)) {
                    auto component = createComponent<TransformComponent>();
                    if (component) {
                        componentManager.AddComponent(entity, std::move(*component));
                    }
                }
            }
        }

        return entity;
    }

    void updateEntity(Entity entity, const Archetype& archetype) 
    {
        // Access the EntityManager and ComponentManager
        EntityManager& entityManager = g_Coordinator.GetEntityManager();
        ComponentManager& componentManager = g_Coordinator.GetComponentManager();

        // Logic to update the entity's components based on the new archetype
        Signature newSignature = createSignature(&archetype);
        entityManager.SetSignature(entity, newSignature);

        // Add new components if necessary
        for (const auto& type : archetype.componentTypes) 
        {
            if (type == typeid(TransformComponent)) 
            {
                // Check if the entity has the component of this type
                if (!componentManager.HaveComponent<TransformComponent>(entity)) {
                    auto component = createComponent<TransformComponent>();
                    if (component) {
                        componentManager.AddComponent(entity, std::move(*component));
                    }
                }
            }
        }
    }

    void updateArchetype(Archetype* archetype, const std::vector<std::type_index>& newComponentTypes) {
        archetype->componentTypes = newComponentTypes; // Update the archetype
        // Optionally update all entities linked to this archetype if needed
    }

    // Method to retrieve all existing archetypes
    std::vector<Archetype*> getArchetypes() 
    {
        std::vector<Archetype*> pointers;
        for (auto& archetype : archetypes) 
        {
            pointers.push_back(&archetype); // Push the address of the archetype
        }
        return pointers;
    }

private:
    std::vector<Archetype> archetypes; // Store all archetypes

    Signature createSignature(const Archetype* archetype) 
    {
        ComponentManager& componentManager = g_Coordinator.GetComponentManager();

        Signature signature = 0; // Assuming Signature is a bitset or a similar type
        for (const auto& type : archetype->componentTypes) {
            // Set the corresponding bit in the signature based on component type
            signature.set(componentManager.GetComponentType(type)); 
            std::cout << "Requested component type: " << type.name() << std::endl;
        }
        return signature;
    }

    template<typename T>
    std::unique_ptr<T> createComponent() 
    {
        return std::make_unique<T>(); // Create and return a unique_ptr to the component
    }
};
