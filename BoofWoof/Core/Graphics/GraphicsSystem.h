#pragma once
#include "../Utilities/Components/GraphicsComponent.h"
//#include "../Core/AssetManager/AssetManager.h"
#include "../ECS/pch.h"
#include "../ECS/System.h"

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>



class GraphicsSystem : public System
{
public:;
    void initGraphicsPipeline(GraphicsComponent& graphicsComponent);
    void UpdateLoop();
    void Draw(std::vector<GraphicsComponent>& components);
    void DrawObject(GraphicsComponent& component);
    
    static bool glewInitialized;
    
    
    void Update(float deltaTime);


    


private:
    static GLFWwindow* newWindow;  // OpenGL window

};

