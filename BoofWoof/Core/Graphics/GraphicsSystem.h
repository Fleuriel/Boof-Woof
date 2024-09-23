#pragma once

#ifndef GRAPHICS_SYSTEM_H
#define GRAPHICS_SYSTEM_H

#include "../Utilities/Components/GraphicsComponent.h"


class GraphicsComponent;

//#include "../Core/AssetManager/AssetManager.h"
#include "../ECS/pch.hpp"
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
    
    
    void Update(float deltaTime);

    void AddModel(std::string const& path);
    void AddObject(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model);
    

private:
    static GLFWwindow* newWindow;  // OpenGL window

};

#endif