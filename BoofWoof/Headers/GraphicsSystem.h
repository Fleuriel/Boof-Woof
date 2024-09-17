#pragma once
#include "GraphicsComponent.h"
#include "pch.h"
#include "System.h"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


class GraphicsSystem : public System
{
public:
    void initWindow();
    static void initGraphicsPipeline(const GraphicsComponent& graphicsComponent);
    static void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);
    void UpdateLoop(std::vector<GraphicsComponent>& components);
    void Draw(std::vector<GraphicsComponent>& components);
    static void DrawObject(GraphicsComponent& component);
    static void CreateObject(OpenGLModel model, int Tag);

    static bool glewInitialized;
    
    
    void Update(glm::vec3 newPosition, glm::vec3 Scale, glm::vec3 angle);


    static GLuint mdl_ref;    // Model reference
    static GLuint shd_ref;    // Shader reference

private:
    static GLFWwindow* newWindow;  // OpenGL window
    static unsigned int VBO, VAO, EBO;
};