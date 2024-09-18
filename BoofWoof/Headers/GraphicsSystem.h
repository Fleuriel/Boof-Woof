#pragma once
#include "GraphicsComponent.h"
#include "WindowComponent.h"
#include "pch.h"
#include "System.h"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>



class GraphicsSystem : public System
{
public:;
    void initGraphicsPipeline(const GraphicsComponent& graphicsComponent);
    void UpdateLoop(GraphicsComponent& graphicsComp);
    void Draw(std::vector<GraphicsComponent>& components);
    void DrawObject(GraphicsComponent& component);
    void CreateObject(OpenGLModel model, int Tag);

    static bool glewInitialized;
    
    
    void Update(float deltaTime);

    void CheckTestsCollisions();
    void UpdateObject(Entity entity, GraphicsComponent& graphicsComp, WindowComponent& windowComp, float deltaTime);


    static GLuint mdl_ref;    // Model reference
    static GLuint shd_ref;    // Shader reference

private:
    static GLFWwindow* newWindow;  // OpenGL window
    static unsigned int VBO, VAO, EBO;
};