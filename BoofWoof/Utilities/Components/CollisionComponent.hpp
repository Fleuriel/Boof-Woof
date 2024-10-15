#pragma once

#ifndef COLLISION_COMPONENT_H
#define COLLISION_COMPONENT_H

#include "ECS/Coordinator.hpp"
#include <glm/glm.hpp>
#include "../Core/Graphics/Model.h"  // Include the model for drawing AABB

class CollisionComponent
{
public:
    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

    CollisionComponent() = default;
    CollisionComponent(const glm::vec3& min, const glm::vec3& max, const Model& aabbModel)
        : aabbMin(min), aabbMax(max), aabbDebugModel(aabbModel) {}

    // Getters
    const glm::vec3& GetMin() const { return aabbMin; }
    const glm::vec3& GetMax() const { return aabbMax; }

    // Setters
    void SetMin(const glm::vec3& min) { aabbMin = min; }
    void SetMax(const glm::vec3& max) { aabbMax = max; }

    // Utility function to check if two AABBs are intersecting
    bool IsCollidingWith(const CollisionComponent& other) const {
        return (aabbMax.x > other.aabbMin.x && aabbMin.x < other.aabbMax.x) &&
            (aabbMax.y > other.aabbMin.y && aabbMin.y < other.aabbMax.y) &&
            (aabbMax.z > other.aabbMin.z && aabbMin.z < other.aabbMax.z);
    }

    // Render the AABB for debug purposes
    void DrawAABBDebug() const {
        aabbDebugModel.DrawCollisionBox3D(aabbDebugModel);
    }

private:
    Entity m_EntityID{};
    glm::vec3 aabbMin;  // Minimum corner of the AABB
    glm::vec3 aabbMax;  // Maximum corner of the AABB
    Model aabbDebugModel;  // AABB model for debug rendering
};

#endif  // COLLISION_COMPONENT_H
