#pragma once

#include "pch.h"
#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" // Include the reflection manager

class BehaviourComponent
{
public:
    // Default constructor
    BehaviourComponent() : m_EntityID(0), m_BehaviourName("Null") {}

    // Parameterized constructor
    BehaviourComponent(std::string behaviourName, Entity& entity)
        : m_BehaviourName(behaviourName), m_EntityID(g_Coordinator.GetEntityId(entity)) {}

    ~BehaviourComponent() = default;

    // Set the Entity ID
    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

    // Getters and setters for the Behaviour Name
    std::string GetBehaviourName() const { return m_BehaviourName; }
    void SetBehaviourName(const std::string& behaviourName) { m_BehaviourName = behaviourName; }

    // Reflection integration
    REFLECT_COMPONENT(BehaviourComponent)
    {
        REGISTER_PROPERTY(BehaviourComponent, BehaviourName, std::string, SetBehaviourName, GetBehaviourName);
    }
    void RegisterPropertiesBehaviour() {
            static bool registered = false;
            if (!registered)
            {
                RegisterAllProperties();  // Call the generated RegisterAllProperties function
                registered = true;
            }
        
    }

private:
    Entity m_EntityID;
    std::string m_BehaviourName;
};
