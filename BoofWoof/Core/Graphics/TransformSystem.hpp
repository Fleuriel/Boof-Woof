#pragma once
#ifndef TRANSFORM_SYSTEM_HPP
#define TRANSFORM_SYSTEM_HPP

#include "ECS/System.hpp"
#include "ECS/Coordinator.hpp"
#include "../Utilities/Components/TransformComponent.hpp"
#include "../Utilities/Components/HierarchyComponent.hpp"
#include <unordered_map>

class TransformSystem : public System
{
public:
    // Map to store calculated world matrices for entities
    std::unordered_map<Entity, glm::mat4> worldMatrices;

    void Update()
    {
        worldMatrices.clear(); // Clear previous frame's data

        for (auto const& entity : g_Coordinator.GetAliveEntitiesSet())
        {
            // If the entity has no parent, it's a root entity
            if (!g_Coordinator.HaveComponent<HierarchyComponent>(entity) ||
                g_Coordinator.GetComponent<HierarchyComponent>(entity).parent == MAX_ENTITIES)
            {
                UpdateTransformRecursive(entity, glm::mat4(1.0f));
            }
        }
    }

    // Get the world matrix of an entity
    const glm::mat4& GetWorldMatrix(Entity entity) const
    {
        auto it = worldMatrices.find(entity);
        if (it != worldMatrices.end())
        {
            return it->second;
        }
        static glm::mat4 identity = glm::mat4(1.0f);
        return identity;
    }

private:
    void UpdateTransformRecursive(Entity entity, const glm::mat4& parentWorldMatrix)
    {
        auto& transform = g_Coordinator.GetComponent<TransformComponent>(entity);

        // Calculate local matrix using existing GetWorldMatrix()
        glm::mat4 localMatrix = transform.GetWorldMatrix();

        // Combine with parent's world matrix to get this entity's world matrix
        glm::mat4 worldMatrix = parentWorldMatrix * localMatrix;

        // Store the calculated world matrix
        worldMatrices[entity] = worldMatrix;

        // Recursively update children
        if (g_Coordinator.HaveComponent<HierarchyComponent>(entity))
        {
            auto& hierarchy = g_Coordinator.GetComponent<HierarchyComponent>(entity);
            for (auto child : hierarchy.children)
            {
                UpdateTransformRecursive(child, worldMatrix);
            }
        }
    }
};

#endif // TRANSFORM_SYSTEM_HPP
