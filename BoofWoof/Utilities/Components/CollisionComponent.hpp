//#pragma once
//
//#ifndef COLLISION_COMPONENT_H
//#define COLLISION_COMPONENT_H
//
//#include "ECS/Coordinator.hpp"
//#include <glm/glm.hpp>
//#include "../Core/Graphics/Model.h"  // Include the model for drawing AABB
//
//class CollisionComponent
//{
//public:
//    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }
//
//    CollisionComponent() = default;
//    CollisionComponent(const glm::vec3& min, const glm::vec3& max, const Model& aabbModel)
//        : aabbMin(min), aabbMax(max), aabbDebugModel(aabbModel) {}
//
//    // Getters
//    const glm::vec3& GetMin() const { return aabbMin; }
//    const glm::vec3& GetMax() const { return aabbMax; }
//
//    // Setters
//    void SetMin(const glm::vec3& min) { aabbMin = min; }
//    void SetMax(const glm::vec3& max) { aabbMax = max; }
//
//    // Utility function to check if two AABBs are intersecting
//    bool IsCollidingWith(const CollisionComponent& other) const {
//        return (aabbMax.x > other.aabbMin.x && aabbMin.x < other.aabbMax.x) &&
//            (aabbMax.y > other.aabbMin.y && aabbMin.y < other.aabbMax.y) &&
//            (aabbMax.z > other.aabbMin.z && aabbMin.z < other.aabbMax.z);
//    }
//
//    // Render the AABB for debug purposes
//    void DrawAABBDebug() const {
//        aabbDebugModel.DrawCollisionBox3D(aabbDebugModel);
//    }
//
//private:
//    Entity m_EntityID{};
//    glm::vec3 aabbMin;  // Minimum corner of the AABB
//    glm::vec3 aabbMax;  // Maximum corner of the AABB
//    Model aabbDebugModel;  // AABB model for debug rendering
//};
//
//#endif  // COLLISION_COMPONENT_H

/**************************************************************************
 * @file CollisionComponent.hpp
 * @author
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

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>                   // For Body
#include <Jolt/Physics/Body/BodyCreationSettings.h>   // For BodyCreationSettings

#include "ECS/Coordinator.hpp"

class CollisionComponent
{
public:
    // Constructor and destructor
    CollisionComponent() = default;
    CollisionComponent(JPH::Body* body, Entity& entity, int layer)
        : m_PhysicsBody(body), m_CollisionLayer(layer), m_EntityID(g_Coordinator.GetEntityId(entity))
    {/*Empty by design*/
    }

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

private:
    Entity m_EntityID{};
    JPH::Body* m_PhysicsBody = nullptr;  // Reference to the JoltPhysics body
    int m_CollisionLayer = 0;  // Layer used for collision filtering
    bool m_HasBodyAdded = false; // Flag to check if the body is already added
};

#endif  // COLLISION_COMPONENT_HPP

