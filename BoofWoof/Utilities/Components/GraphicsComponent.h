#pragma once
#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "ECS/Coordinator.hpp"
#include "../Core/Graphics/Model.h"   // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here

class GraphicsComponent 
{
public:

    // set position
    void setPosition(Object& object, glm::vec3 position) { object.position = position; }

    // set scale
    void setScale(Object& object, glm::vec3 scale) { object.scale = scale; }

    // set rotation
    void setRotation(Object& object, glm::vec3 rotation) { object.rotation = rotation; }

    // set color
    void setColor(Object& object, glm::vec3 color) { object.color = color; }

    // set model
    void setModel(Object& object, Model* model) { object.model = model; }

    // Constructor and destructor
    GraphicsComponent();
    ~GraphicsComponent() = default;
};

#endif  // GRAPHICS_COMPONENT_H
