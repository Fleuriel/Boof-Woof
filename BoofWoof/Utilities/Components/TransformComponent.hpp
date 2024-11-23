#pragma once
#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Graphics/Model.h"   // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here
#include "../Core/Reflection/ReflectionManager.hpp"      // Include the reflection system
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


class TransformComponent
{
public:
    // Constructor and destructor
    TransformComponent() = default;
    TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Entity& entity)
        : m_Position(position), m_Scale(scale), m_Rotation(rotation), m_EntityID(g_Coordinator.GetEntityId(entity)) {}

    ~TransformComponent() = default;

    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

    // Setter methods
    void SetPosition(glm::vec3 position) { m_Position = position; }
    void SetScale(glm::vec3 scale) { m_Scale = scale; }
    void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; }

    // Getter methods
    glm::vec3& GetPosition() { return m_Position; }
    glm::vec3& GetScale() { return m_Scale; }
    glm::vec3& GetRotation() { return m_Rotation; }

    glm::mat4 GetWorldMatrix()
    {
        // Create scale matrix
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), m_Scale);

        // Create rotation matrix using quaternions for proper rotation handling
        glm::quat quaternion = glm::quat(m_Rotation); // m_Rotation should be in radians
        
        glm::mat4 rotateMat = glm::toMat4(quaternion);

        // Create translation matrix
        glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), m_Position);

        // Combine transformations: ModelMatrix = Translation * Rotation * Scale
        glm::mat4 worldMatrix = translateMat * rotateMat * scaleMat;
        return worldMatrix;
    }

    void SetTransformation(const glm::mat4& transform) {
        // Extract the position from the transformation matrix
        m_Position = glm::vec3(transform[3]);

        // Extract the scale
        m_Scale = glm::vec3(
            glm::length(glm::vec3(transform[0])),
            glm::length(glm::vec3(transform[1])),
            glm::length(glm::vec3(transform[2]))
        );

        // Extract the rotation by removing the scaling from the rotation part of the matrix
        glm::mat3 rotationMat = glm::mat3(transform);
        rotationMat[0] /= m_Scale.x;
        rotationMat[1] /= m_Scale.y;
        rotationMat[2] /= m_Scale.z;

        m_Rotation = glm::eulerAngles(glm::quat_cast(rotationMat)); // Convert to Euler angles
    }



    // Reflection integration
    REFLECT_COMPONENT(TransformComponent)
    {
        REGISTER_PROPERTY(TransformComponent, Position, glm::vec3, SetPosition, GetPosition);
        REGISTER_PROPERTY(TransformComponent, Scale, glm::vec3, SetScale, GetScale);
        REGISTER_PROPERTY(TransformComponent, Rotation, glm::vec3, SetRotation, GetRotation);
    }

private:
    Entity m_EntityID{};
    glm::vec3 m_Position{};
    glm::vec3 m_Scale{ 1.0f, 1.0f, 1.0f };
    glm::vec3 m_Rotation{};
};

#endif // TRANSFORM_COMPONENT_HPP
