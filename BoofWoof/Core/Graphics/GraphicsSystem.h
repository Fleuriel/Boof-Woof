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



class GraphicsSystem : public System
{
public:;
    void initGraphicsPipeline();
    void UpdateLoop();
    void Draw(std::vector<GraphicsComponent>& components);
    void DrawObject(GraphicsComponent& component);
    
    static bool glewInitialized;
    
    GLuint GetFramebufferTexture() const { return textureColorbuffer; }  // Getter function
    void Update(float deltaTime);

    void AddModel_3D(std::string const& path);
    void AddObject_3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model);

    void AddModel_2D();

    void AddObject_2D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model2D model);
    void UpdateViewportSize(int width, int height);  // Method to handle viewport resizing

private:
    unsigned int fbo;
    unsigned int textureColorbuffer;  // Store the framebuffer texture ID here
    unsigned int rbo;
    static GLFWwindow* newWindow;  // OpenGL window

};

#endif