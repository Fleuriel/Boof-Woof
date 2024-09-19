#include "GraphicsSystem.h"
#include "Shader.h"
#include <utility>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <unordered_map>
#include "../ECS/pch.h"
#include "../Input/Input.h"
#include "../AssetManager/AssetManager.h"

#include "Windows/WindowManager.h"

// Assignment 1
#include "BoundingVolume.h"
#include "TestCases.h"






bool GraphicsSystem::glewInitialized = false;



void GraphicsSystem::initGraphicsPipeline(const GraphicsComponent& graphicsComponent) {
    // Implement graphics pipeline initialization
		// OpenGL Initialization
	std::cout << "Initializing Graphics Pipeline\n";

	if (!glewInitialized)
	{
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
			return;
		}
		glewInitialized = true;
	}


	// load shaders
	g_AssetManager.LoadShaders();


}




void GraphicsSystem::UpdateLoop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);


}



void GraphicsSystem::Draw(std::vector<GraphicsComponent>& components) {
    // Loop through components and draw them
    for (auto& component : components) {
        DrawObject(component);
    }
}

void GraphicsSystem::DrawObject(GraphicsComponent& component) {
    // Draw logic using component data
}


