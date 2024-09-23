#include "GraphicsSystem.h"
#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "../Input/Input.h"
#include "Camera.h"

#include "AssetManager/AssetManager.h"
#include "Windows/WindowManager.h"

// Assignment 1


bool GraphicsSystem::glewInitialized = false;


Camera		camera;


void GraphicsSystem::initGraphicsPipeline() {
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
	AddModel_3D("sphere.obj");


	AddModel_2D();

	// load objects
	AddObject_3D(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), &g_AssetManager.Models[0]);

	AddObject_2D(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), g_AssetManager.Model2D[0]);



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
		//object.model->Draw(g_AssetManager.shdrpgms[0]);

	}

	g_AssetManager.shdrpgms[0].UnUse();

	g_AssetManager.shdrpgms[1].Use();


	std::cout << g_AssetManager.Model2D[0].vaoid << '\n';

	for (auto& object : g_AssetManager.Object2D)
	{
		

		g_AssetManager.shdrpgms[1].SetUniform("uModel_to_NDC_2D", object.model_to_NDC_xform_2D);
		object.UpdateObject2D(0.1f);
		object.model->Draw(g_AssetManager.shdrpgms[1]);


	}

	g_AssetManager.shdrpgms[1].UnUse();


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


void GraphicsSystem::AddModel_3D(std::string const& path)
{
	Model model;
	model.loadModel(path, GL_TRIANGLES);
	g_AssetManager.Models.push_back(model);
}

void GraphicsSystem::AddObject_3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model)
{
	Object object;
	object.model = model;
	object.position = position;
	object.scale = scale;
	object.rotation = rotation;
	object.color = color;
	g_AssetManager.Objects.push_back(object);
}



void GraphicsSystem::AddModel_2D()
{
	Model2D model;

	model = SquareModel(glm::vec3(0.0f));

	g_AssetManager.Model2D.push_back(model);
}

void GraphicsSystem::AddObject_2D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model2D model)
{
	Object2D obj2D;

	obj2D.model = &model;
	obj2D.position = position;
	obj2D.scale = scale;
	obj2D.rotation = rotation;
	obj2D.color = color;

	g_AssetManager.Object2D.push_back(obj2D);
}


