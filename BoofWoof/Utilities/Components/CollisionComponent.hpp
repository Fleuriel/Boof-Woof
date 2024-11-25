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
    //CollisionComponent(int layer, bool isDynamic = false)
    //    : m_CollisionLayer(layer), m_IsDynamic(isDynamic) {}
    // Constructor with initial AABB size
    CollisionComponent(int layer, bool isDynamic = false, const glm::vec3& initialAABB = glm::vec3(1.0f, 1.0f, 1.0f))
        : m_CollisionLayer(layer), m_IsDynamic(isDynamic), m_AABBSize(initialAABB) {}

    ~CollisionComponent() = default;

    // Setters
    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }
    void SetPhysicsBody(JPH::Body* body) { m_PhysicsBody = body; }
    void SetCollisionLayer(int layer) { m_CollisionLayer = layer; }
    void SetHasBodyAdded(bool hasBodyAdded) { m_HasBodyAdded = hasBodyAdded; }
    void SetIsDynamic(bool isDynamic) { m_IsDynamic = isDynamic; }
    void SetIsPlayer(bool isPlayer) { m_IsPlayer = isPlayer; }
    void SetIsColliding(bool value) { isColliding = value; }

    // Getters
    JPH::Body* GetPhysicsBody() const { return m_PhysicsBody; }
    int GetCollisionLayer() const { return m_CollisionLayer; }
    bool HasBodyAdded() const { return m_HasBodyAdded; }
    bool IsDynamic() const { return m_IsDynamic; }
    bool IsPlayer() const { return m_IsPlayer; }
    bool GetIsColliding() const { return isColliding; }

    // AABB Size Getters and Setters
    void SetAABBSize(const glm::vec3& size) 
    { 
        m_AABBSize = size;
    }
    glm::vec3 GetAABBSize() const { return m_AABBSize; }

    glm::vec3 GetScaledAABBSize(const glm::vec3& scale) const {
        return m_AABBSize * scale; // Compute the scaled AABB dynamically
    }

    // Getter and setter for offset
    glm::vec3 GetAABBOffset() const { return AABBOffset; }
    void SetAABBOffset(const glm::vec3& newOffset) { AABBOffset = newOffset; }

    void SetLastCollidedObjectName(const std::string& objectName) { lastCollidedObjectName = objectName; }
    const std::string& GetLastCollidedObjectName() const { return lastCollidedObjectName; }

    // Reflection integration
    REFLECT_COMPONENT(CollisionComponent)
    {
        REGISTER_PROPERTY(CollisionComponent, CollisionLayer, int, SetCollisionLayer, GetCollisionLayer);
        REGISTER_PROPERTY(CollisionComponent, HasBodyAdded, bool, SetHasBodyAdded, HasBodyAdded);
        REGISTER_PROPERTY(CollisionComponent, IsDynamic, bool, SetIsDynamic, IsDynamic);
        REGISTER_PROPERTY(CollisionComponent, IsPlayer, bool, SetIsPlayer, IsPlayer);
        REGISTER_PROPERTY(CollisionComponent, GetIsColliding, bool, SetIsColliding, GetIsColliding);
        REGISTER_PROPERTY(CollisionComponent, AABBOffset, glm::vec3, SetAABBOffset, GetAABBOffset); // Add this line
    }

private:
    Entity m_EntityID{};
    JPH::Body* m_PhysicsBody = nullptr;  // Reference to the JoltPhysics body
    int m_CollisionLayer = 0;            // Layer used for collision filtering
    bool m_HasBodyAdded = false;         // Flag to check if the body is already added
    glm::vec3 m_AABBSize = glm::vec3(1.0f, 1.0f, 1.0f); // Default AABB size (x, y, z)
    glm::vec3 AABBOffset = glm::vec3(0.0f); // Default to no offset
    bool m_IsDynamic = false;             // Flag to determine if the entity is dynamic or static
    bool m_IsPlayer = false;            // Determines if the entity is a player
    bool isColliding = false; // Flag to track collision status
    std::string lastCollidedObjectName = "None"; // Default to no collision
};

#endif  // COLLISION_COMPONENT_HPP

#pragma warning(pop)
