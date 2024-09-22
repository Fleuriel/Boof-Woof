#pragma once
#include "../Core/ECS/pch.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "../Core/Graphics/Model.h"
#include "../Core/Graphics/Object.h"

class RenderTest
{
public:
    RenderTest() {}        // basic constructor

    RenderTest() {/*Empty by design*/ }


private:

    glm::vec3 m_Color{};
    glm::vec3 m_Position{};
    glm::vec3 m_Scale{};
    glm::vec3 m_Rotation{};

    Model* m_Model{};

    Entity m_EntityID{};
};