#pragma once
#ifndef RENDERTEST_H
#define RENDERTEST_H

class Coordinator;
class Model;
class Object;

#include <glm/glm.hpp>

class RenderTest
{
public:
    RenderTest() {}        // basic constructor

    RenderTest(Model* model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Entity& entity) 
        : m_Model(model), m_Position(position), m_Scale(scale), m_Rotation(rotation), m_Color(color), m_EntityID(g_Coordinator.GetEntityId(entity)) {/*Empty by design*/ }

    void SetComponentEntityID(Entity& entity) { m_EntityID = entity; }

    // setters
    void SetColor(glm::vec3 color)
    {
        m_Color = color;
    }

    void SetPosition(glm::vec3 pos)
    {
        m_Position = pos;
    }

    void SetScale(glm::vec3 scale)
    {
        m_Scale = scale;
    }

    void SetRotation(glm::vec3 rotate)
    {
        m_Rotation = rotate;
    }

    void SetModel(glm::vec3 rotate)
    {
        m_Rotation = rotate;
    }

    // Getters
    glm::vec3& GetColor() { return m_Color; };
    glm::vec3& GetPosition() { return m_Position; };
    glm::vec3& GetScale() { return m_Scale; };
    glm::vec3& GetRotation() { return m_Rotation; };
    Model* getModel() const { return m_Model; };

private:

    glm::vec3 m_Color{};
    glm::vec3 m_Position{};
    glm::vec3 m_Scale{};
    glm::vec3 m_Rotation{};

    Model* m_Model{};

    Entity m_EntityID{};
};

#endif  // RENDERTEST_H