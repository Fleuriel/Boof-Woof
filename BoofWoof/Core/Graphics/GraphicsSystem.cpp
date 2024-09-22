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
#include "Camera.h"
#include "../Windows/WindowManager.h"
#include "Windows/WindowManager.h"

// Assignment 1


bool GraphicsSystem::glewInitialized = false;

//std::vector<Model2D> models;

Camera		camera;


void GraphicsSystem::initGraphicsPipeline(GraphicsComponent& graphicsComponent) {
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
	//model_try.loadModel("sphere.obj", GL_TRIANGLES);

	// load one object
	/*object_try.model = &model_try;
	object_try.position = glm::vec3(0.0f, 0.0f, 0.0f);
	object_try.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	object_try.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	object_try.color = glm::vec3(1.0f, 1.0f, 1.0f);

	g_AssetManager.Models.push_back(model_try);
	g_AssetManager.Objects.push_back(object_try);*/

	// store the model
	//models.emplace_back(SquareModel(glm::vec3(1.0f, 1.0f, 1.0f)));

	//Model2D createModel;

//	objects.emplace_back(createModel);

	// initialize the model


	// load models
	graphicsComponent.addModel("sphere.obj");

	// load objects
	graphicsComponent.addObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), &g_AssetManager.Models[0]);


	//init camera
	camera = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

}




void GraphicsSystem::UpdateLoop() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);


	// emply matrix
	glm::mat4 mtx = glm::mat4(1.0f);

	// camera matrix
	// camera initial position is (0, 0, 3)
	//camera.ProcessKeyboard(Camera_Movement::FORWARD, -0.001f);
	glm::mat4 view_ = camera.GetViewMatrix();
	//std::cout << "camera position: " << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z << std::endl;
	
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)g_WindowX/ (float)g_WindowY, 0.1f, 100.0f);

	


	// Draw the object
	g_AssetManager.shdrpgms[0].Use();
	for (auto& object : g_AssetManager.Objects)
	{
		g_AssetManager.shdrpgms[0].SetUniform("vertexTransform", object.getWorldMatrix());
		g_AssetManager.shdrpgms[0].SetUniform("view", view_);
		g_AssetManager.shdrpgms[0].SetUniform("projection", projection);
		g_AssetManager.shdrpgms[0].SetUniform("objectColor", object.color);
		object.model->Draw(g_AssetManager.shdrpgms[0]);

	}


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


