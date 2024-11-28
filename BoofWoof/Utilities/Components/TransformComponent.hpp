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
    TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 rotationPivotOffset, Entity& entity)
        : m_Position(position), m_Scale(scale), m_Rotation(rotation), m_RotationPivotOffset(rotationPivotOffset), m_EntityID(g_Coordinator.GetEntityId(entity)) {}

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

    //glm::mat4 GetWorldMatrix()
    //{
    //    // Create scale matrix
    //    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), m_Scale);

    //    // Create rotation matrix using quaternions for proper rotation handling
    //    glm::quat quaternion = glm::quat(m_Rotation); // m_Rotation should be in radians
    //    
    //    glm::mat4 rotateMat = glm::toMat4(quaternion);

    //    // Create translation matrix
    //    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), m_Position);

    //    // Combine transformations: ModelMatrix = Translation * Rotation * Scale
    //    glm::mat4 worldMatrix = translateMat * rotateMat * scaleMat;
    //    return worldMatrix;
    //}

    glm::mat4 GetWorldMatrix()
    {
        // Create scale matrix
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), m_Scale);

        // Create rotation matrix
        glm::quat quaternion = glm::quat(m_Rotation);
        glm::mat4 rotateMat = glm::toMat4(quaternion);

        // Step 1: Use m_RotationPivotOffset as the offset
        glm::vec3 pivotOffset = m_RotationPivotOffset; // Dynamically adjust pivot offset
        glm::mat4 offsetTranslate = glm::translate(glm::mat4(1.0f), pivotOffset);

        // Step 2: Apply rotation
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_Position); // Translation to position
        modelMatrix *= offsetTranslate;  // Move pivot to offset
        modelMatrix *= rotateMat;        // Apply rotation
        modelMatrix *= glm::translate(glm::mat4(1.0f), -pivotOffset); // Translate back

        // Step 3: Apply scale
        modelMatrix *= scaleMat;

        return modelMatrix;
    }

    void SetRotationPivotOffset(const glm::vec3& offset) { m_RotationPivotOffset = offset; }
    glm::vec3 GetRotationPivotOffset() const { return m_RotationPivotOffset; }

    // Editing flag
    void SetEditing(bool editing) { m_IsEditing = editing; }
    bool IsEditing() const { return m_IsEditing; }

    // Reflection integration
    REFLECT_COMPONENT(TransformComponent)
    {
        REGISTER_PROPERTY(TransformComponent, Position, glm::vec3, SetPosition, GetPosition);
        REGISTER_PROPERTY(TransformComponent, Scale, glm::vec3, SetScale, GetScale);
        REGISTER_PROPERTY(TransformComponent, Rotation, glm::vec3, SetRotation, GetRotation);
        REGISTER_PROPERTY(TransformComponent, RotationPivotOffset, glm::vec3, SetRotationPivotOffset, GetRotationPivotOffset);
    }

private:
    Entity m_EntityID{};
    glm::vec3 m_Position{};
    glm::vec3 m_Scale{ 1.0f, 1.0f, 1.0f };
    glm::vec3 m_Rotation{};
    bool m_IsEditing = false; // New flag for editing state
    glm::vec3 m_RotationPivotOffset; // New property for pivot offset
};

#endif // TRANSFORM_COMPONENT_HPP
