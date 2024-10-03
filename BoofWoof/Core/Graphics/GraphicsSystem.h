#pragma once

#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "../Utilities/Components/GraphicsComponent.hpp"
#include "../Utilities/Components/TransformComponent.hpp"

class Model;
class Model2D;
class GraphicsComponent;

#include "../ECS/System.hpp"

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>




struct ShaderParams {

    glm::mat4 WorldMatrix;
    glm::mat4 View;
    glm::mat4 Projection;
    glm::vec3 Color;


    ShaderParams() : WorldMatrix(glm::mat4(0.0f)), View(glm::mat4(0.0f)), Projection(glm::mat4(0.0f)), Color(glm::vec3(0.1f, 0.2f, 0.3f)) {}

};


class GraphicsSystem : public System
{
public:;
    void initGraphicsPipeline();
    void UpdateLoop();
    void Draw(std::vector<GraphicsComponent>& components);
    void DrawObject(GraphicsComponent& component);
    
    static bool glewInitialized;
    static bool debug;
    static bool D2; // 0 is 2D, 1 is 3D
    static bool D3; // 0 is 2D, 1 is 3D
    
    void SetShaderUniforms(OpenGLShader& shader, const ShaderParams& shdrParam);


    GLuint GetFramebufferTexture() const { return textureColorbuffer; }  // Getter function
    void Update(float deltaTime);

    void AddModel_3D(std::string const& path);
    void AddObject_3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model);

    void AddModel_2D();

    void UpdateViewportSize(int width, int height);  // Method to handle viewport resizing
    inline void SetEditorMode(bool EditorMode) { editorMode = EditorMode; };

private:
    unsigned int fbo;
    unsigned int textureColorbuffer;  // Store the framebuffer texture ID here
    unsigned int rbo;
    bool editorMode = false;
    static GLFWwindow* newWindow;  // OpenGL window

};

#endif