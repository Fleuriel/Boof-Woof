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
#include "Object.h"
#include "Model.h"
#include "Windows/WindowManager.h"

// Assignment 1


bool GraphicsSystem::glewInitialized = false;

Model model_try;
Object object_try;



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

	// load one model
	model_try.loadModel("sphere.obj", GL_TRIANGLES);

	// load one object
	object_try.model = &model_try;
	object_try.position = glm::vec3(0.0f, 0.0f, 0.0f);
	object_try.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	object_try.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	object_try.color = glm::vec3(1.0f, 0.0f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

}




void GraphicsSystem::UpdateLoop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

	// emply matrix
	glm::mat4 mtx = glm::mat4(1.0f);
	// Draw the object
	g_AssetManager.shdrpgms[0].Use();


	g_AssetManager.shdrpgms[0].SetUniform("vertexTransform", object_try.getWorldMatrix());
	g_AssetManager.shdrpgms[0].SetUniform("view", mtx);
	g_AssetManager.shdrpgms[0].SetUniform("projection", mtx);
	g_AssetManager.shdrpgms[0].SetUniform("objectColor", object_try.color);
//	g_AssetManager.shdrpgms[0].SetUniform("lineRender", false);

	object_try.model->Draw(g_AssetManager.shdrpgms[0]);

	g_AssetManager.shdrpgms[0].UnUse();

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


