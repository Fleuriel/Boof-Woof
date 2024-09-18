#pragma once
#include "../Core/ECS/pch.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>


struct OpenGLModel {
    std::string Name;
    GLenum primitive_type;
    size_t primitive_cnt;
    GLuint vaoid;
    GLsizei draw_cnt;
    size_t idx_elem_cnt;

    OpenGLModel() : primitive_type(0), primitive_cnt(0), vaoid(0), draw_cnt(0), idx_elem_cnt(0) {}
};

class GraphicsComponent {
public:
    glm::mat4 model_To_NDC_xform;   // Model to NDC transform
    glm::vec3 Position;             // Position
    glm::vec3 ScaleModel;           // Scale
    glm::vec3 setColor;             // Color
    glm::vec3 setColorLeft;         // Color Left
    glm::vec3 setColorRight;        // Color Right
    glm::vec3 PositionLeft;         // Position Left
    glm::vec3 PositionRight;        // Position Right
    glm::vec3 AngleLeft;            // Angle Left
    glm::vec3 AngleRight;           // Angle Right
    glm::vec3 Angle;
    float angleSpeed;               // Rotation Speed
    int TagID;                      // Tag ID
    OpenGLModel model_to_create;    // Model to create
    float size;                     // Size

    // Constructor and destructor
    GraphicsComponent();
//    GraphicsComponent(const OpenGLModel& model, int id = 0);
    ~GraphicsComponent() = default;
};

extern std::list<GraphicsComponent> objects;