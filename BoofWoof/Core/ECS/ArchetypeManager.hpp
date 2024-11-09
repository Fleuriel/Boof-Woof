#pragma once

#include "pch.h"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"

#define g_Arch ArchetypeManager::GetInstance()

class Archetype 
{
public:
    std::string name; // Name of the archetype
    std::vector<std::type_index> componentTypes; // Store component types
    std::vector<Entity> archEntities; // Keep track of entities created from this archetype

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

    // Function to add a component
    template<typename T>
    void addComponents(Entity entity, ComponentManager& componentManager) 
    {
        if (!componentManager.HaveComponent<T>(entity)) {
            auto component = createComponent<T>();
            if (component) {
                componentManager.AddComponent(entity, std::move(*component));
            }
        }
    }

    Entity createEntity(Archetype* archetype)
    {
        Entity entity = entityManager.CreateEntity();

        // Add the entity to the archetype's entity list
        archetype->archEntities.push_back(entity);

        // Set the entity's signature based on the archetype
        Signature signature = createSignature(archetype);
        entityManager.SetSignature(entity, signature);

        // Always have MetadataComponent and set object name to Archetype name
        addComponents<MetadataComponent>(entity, componentManager);
        componentManager.GetComponent<MetadataComponent>(entity).SetName(archetype->name);

        // Initialize components for the new entity based on the archetype
        for (const auto& type : archetype->componentTypes) 
        {
            //std::cout << "Processing component type: " << type.name() << std::endl;

            if (type == typeid(TransformComponent)) addComponents<TransformComponent>(entity, componentManager);
            if (type == typeid(GraphicsComponent)) addComponents<GraphicsComponent>(entity, componentManager);           
            if (type == typeid(CollisionComponent)) addComponents<CollisionComponent>(entity, componentManager);  
            if (type == typeid(AudioComponent)) addComponents<AudioComponent>(entity, componentManager);
        }

        return entity;
    }

    void updateArchetype(Archetype* archetype, const std::vector<std::type_index>& newComponentTypes) 
    {
        archetype->componentTypes = newComponentTypes; // Update the archetype

        // Check if the entity's signature matches the updated archetype
        Signature newSignature = createSignature(archetype);

        // Iterate through all entities to update their components based on the new archetype
        for (const auto& entity : archetype->archEntities)
        {
            // Update the entity's signature
            entityManager.SetSignature(entity, newSignature);

            // Always ensure the MetadataComponent is present
            if (!componentManager.HaveComponent<MetadataComponent>(entity)) {
                addComponents<MetadataComponent>(entity, componentManager);
            }
            componentManager.GetComponent<MetadataComponent>(entity).SetName(archetype->name); // Update name

            // Initialize or update components for the entity based on the new archetype
            for (const auto& type : archetype->componentTypes) 
            {
                if (type == typeid(TransformComponent)) addComponents<TransformComponent>(entity, componentManager);
                if (type == typeid(GraphicsComponent)) addComponents<GraphicsComponent>(entity, componentManager);
                if (type == typeid(CollisionComponent)) addComponents<CollisionComponent>(entity, componentManager);
                if (type == typeid(AudioComponent)) addComponents<AudioComponent>(entity, componentManager);
            }
        }
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

    // Access the EntityManager and ComponentManager
    EntityManager& entityManager = g_Coordinator.GetEntityManager();
    ComponentManager& componentManager = g_Coordinator.GetComponentManager();

    Signature createSignature(const Archetype* archetype) 
    {
        ComponentManager& componentManager = g_Coordinator.GetComponentManager();

        Signature signature = 0; // Assuming Signature is a bitset or a similar type
        for (const auto& type : archetype->componentTypes) 
        {
            // Set the corresponding bit in the signature based on component type
            signature.set(componentManager.GetComponentType(type)); 
        }
        return signature;
    }

    template<typename T>
    std::unique_ptr<T> createComponent()
    {
        return std::make_unique<T>(); // Create and return a unique_ptr to the component
    }
};
