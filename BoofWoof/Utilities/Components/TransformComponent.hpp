#pragma once
#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Graphics/Model.h"   // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here
#include "../Core/Property/MyProperty.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TransformComponent
{
public:
    // Constructor and destructor
    TransformComponent() {};
    TransformComponent(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, Entity& entity)
        : m_Position(position), m_Scale(scale), m_Rotation(rotation),
        m_EntityID(g_Coordinator.GetEntityId(entity)) {}

    ~TransformComponent() = default;

    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

    // Setters
    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetScale(const glm::vec3& scale) { m_Scale = scale; }
    void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

    // Getters
    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetScale() const { return m_Scale; }
    const glm::vec3& GetRotation() const { return m_Rotation; }

    // Compute the world matrix
    glm::mat4 GetWorldMatrix() {
        glm::mat4 worldMatrix = glm::mat4(1.0f);

        worldMatrix = glm::rotate(worldMatrix, m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        worldMatrix = glm::rotate(worldMatrix, m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        worldMatrix = glm::rotate(worldMatrix, m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 scaleMat = glm::mat4(1.0f);
        scaleMat = glm::scale(scaleMat, m_Scale);

        glm::mat4 translateMat = glm::mat4(1.0f);
        translateMat = glm::translate(translateMat, m_Position);

        worldMatrix = translateMat * worldMatrix * scaleMat;

        return worldMatrix;
    }

    // Set default values
    void setValues()
    {
        m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
        m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void CheckValues() const
    {
        assert(m_Position == glm::vec3(0.0f, 0.0f, 0.0f));
        assert(m_Scale == glm::vec3(1.0f, 1.0f, 1.0f));
        assert(m_Rotation == glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // Define the properties using XPROPERTY_DEF
    XPROPERTY_DEF(
        "TransformComponent", TransformComponent,
        obj_member<"Position", &TransformComponent::m_Position>,
        obj_member<"Scale", &TransformComponent::m_Scale>,
        obj_member<"Rotation", &TransformComponent::m_Rotation>,
        obj_member<"setValues", &TransformComponent::setValues>,
        obj_member<"CheckValues", &TransformComponent::CheckValues>
    );

private:
    Entity m_EntityID{};
    glm::vec3 m_Position{};
    glm::vec3 m_Scale{ 1.f, 1.f, 1.f };
    glm::vec3 m_Rotation{};
};

// Register the properties using XPROPERTY_REG
XPROPERTY_REG(TransformComponent)

#endif  // TRANSFORM_COMPONENT_HPP
