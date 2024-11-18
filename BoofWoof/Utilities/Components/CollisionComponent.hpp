/**************************************************************************
 * @file CollisionComponent.hpp
 * @param DP email:
 * @param Course: CS 3401
 * @param Course: Game Project 3
 * @date 17/10/2024
 * @brief
 *
 * This file contains the definition of the CollisionComponent class, which is used to
 * store the physics body and collision-related data of an entity.
 *
 *************************************************************************/

#pragma once
#ifndef COLLISION_COMPONENT_HPP
#define COLLISION_COMPONENT_HPP

#pragma warning(push)
#pragma warning(disable: 6385 6386)

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>                   // For Body
#include <Jolt/Physics/Body/BodyCreationSettings.h>   // For BodyCreationSettings
#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"   // Include the reflection manager

class CollisionComponent
{
public:
    // Constructor and destructor
    CollisionComponent() = default;
    CollisionComponent(JPH::Body* body, Entity& entity, int layer)
        : m_PhysicsBody(body), m_CollisionLayer(layer), m_EntityID(g_Coordinator.GetEntityId(entity))
    {
        /*Empty by design*/
    }
    CollisionComponent(int layer) : m_CollisionLayer(layer) {}

    ~CollisionComponent() = default;

    // Setters
    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }
    void SetPhysicsBody(JPH::Body* body) { m_PhysicsBody = body; }
    void SetCollisionLayer(int layer) { m_CollisionLayer = layer; }
    void SetHasBodyAdded(bool hasBodyAdded) { m_HasBodyAdded = hasBodyAdded; }

    // Getters
    JPH::Body* GetPhysicsBody() const { return m_PhysicsBody; }
    int GetCollisionLayer() const { return m_CollisionLayer; }
    bool HasBodyAdded() const { return m_HasBodyAdded; }

    // AABB Size Getters and Setters
    void SetAABBSize(const glm::vec3& size) { m_AABBSize = size; }
    glm::vec3 GetAABBSize() const { return m_AABBSize; }

    // Reflection integration
    REFLECT_COMPONENT(CollisionComponent)
    {
        REGISTER_PROPERTY(CollisionComponent, CollisionLayer, int, SetCollisionLayer, GetCollisionLayer);
        REGISTER_PROPERTY(CollisionComponent, HasBodyAdded, bool, SetHasBodyAdded, HasBodyAdded);
    }

private:
    Entity m_EntityID{};
    JPH::Body* m_PhysicsBody = nullptr;  // Reference to the JoltPhysics body
    int m_CollisionLayer = 0;            // Layer used for collision filtering
    bool m_HasBodyAdded = false;         // Flag to check if the body is already added
    glm::vec3 m_AABBSize = glm::vec3(1.0f, 1.0f, 1.0f); // Default AABB size (x, y, z)
};

#endif  // COLLISION_COMPONENT_HPP

#pragma warning(pop)
