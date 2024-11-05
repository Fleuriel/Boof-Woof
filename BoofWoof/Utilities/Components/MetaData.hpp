#pragma once
#ifndef METADATA_COMPONENT_H
#define METADATA_COMPONENT_H

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" // Include the reflection manager

class MetadataComponent
{
public:
    // Constructors
    MetadataComponent() = default;
    MetadataComponent(std::string name, Entity& entity)
        : m_Name(name), m_EntityID(g_Coordinator.GetEntityId(entity)) { /*empty by design*/
    }

    // Set the Entity ID
    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

    // Getters and setters for the name
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& newName) { m_Name = newName; }

    // Reflection integration
    REFLECT_COMPONENT(MetadataComponent)
    {
        REGISTER_PROPERTY(MetadataComponent, Name, std::string, SetName, GetName);
    }

private:
    Entity m_EntityID{};
    std::string m_Name{};
};

#endif  // METADATA_COMPONENT_H
