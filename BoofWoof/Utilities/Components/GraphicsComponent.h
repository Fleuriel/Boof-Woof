#pragma once
#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "../Core/ECS/pch.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
#include "../Core/Graphics/Model.h"  // Make sure Model is included
#include "../Core/Graphics/Object.h"  // Full definition of Object is needed here

class GraphicsComponent {
public:
    // add model
    void addModel(const std::string& path);

    // add object
    void addObject(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model);

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
