#pragma once
#include "../Core/ECS/pch.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>
//#include "Object.h"



class GraphicsComponent {
public:
    // Constructor and destructor
    GraphicsComponent();
//    GraphicsComponent(const OpenGLModel& model, int id = 0);
    ~GraphicsComponent() = default;
};

extern std::list<GraphicsComponent> objects;