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

//std::vector<Model2D> models;

Camera		camera;


void GraphicsSystem::initGraphicsPipeline() {
	// Implement graphics pipeline initialization
	std::cout << "Initializing Graphics Pipeline\n";


	// Continue with other initialization (shaders, models, etc.)
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
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create a texture for the framebuffer
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_WindowX, g_WindowY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// Create a renderbuffer object for depth and stencil attachment
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_WindowX, g_WindowY);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check if the framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is not complete!" << std::endl;

	// Unbind the framebuffer to render to the default framebuffer initially
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// load shaders and models
	g_AssetManager.LoadShaders();
	AddModel_3D("../BoofWoof/sphere.obj");
	AddModel_2D();

	// Initialize camera
	camera = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}





void GraphicsSystem::UpdateLoop() {
	// Bind the framebuffer for rendering
	if(editorMode == true)
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear framebuffer

	// Setup camera and projection matrix
	glm::mat4 view_ = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)g_WindowX / (float)g_WindowY, 0.1f, 100.0f);

	g_AssetManager.shdrpgms[0].Use();

	// Loop through all entities and render them
	auto allEntities = g_Coordinator.GetAliveEntitiesSet();
	for (auto& entity : allEntities)
	{
		if (g_Coordinator.HaveComponent<TransformComponent>(entity))
		{
			auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
			if (g_Coordinator.HaveComponent<GraphicsComponent>(entity))
			{
				auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
				if (graphicsComp.getModel() == nullptr)
				{
					std::cout << "Model is null" << std::endl;
					graphicsComp.SetModel(&g_AssetManager.ModelMap["sphere"]);
					continue;
				}
				g_AssetManager.shdrpgms[0].SetUniform("vertexTransform", transformComp.GetWorldMatrix());
				g_AssetManager.shdrpgms[0].SetUniform("view", view_);
				g_AssetManager.shdrpgms[0].SetUniform("projection", projection);
				g_AssetManager.shdrpgms[0].SetUniform("objectColor", glm::vec3{ 1.0f });
				graphicsComp.getModel()->Draw(g_AssetManager.shdrpgms[0]);
			}
		}
	}

	g_AssetManager.shdrpgms[0].UnUse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind the framebuffer to switch back to the default framebuffer
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
	std::cout << "Loading: " << path << '\n';
	
	model.loadModel(path, GL_TRIANGLES);

	std::string name = path.substr(path.find_last_of('/') + 1);
	//remove .obj from name
	name = name.substr(0, name.find_last_of('.'));

	g_AssetManager.ModelMap.insert(std::pair<std::string, Model>(name, model));


	std::cout << "Loaded: " << path<<" with name: "<<name << " [Models Reference: " << g_AssetManager.ModelMap.size() - 1 << "]" << '\n';
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


void GraphicsSystem::UpdateViewportSize(int width, int height) {
	if (width == 0 || height == 0) {
		// Avoid updating the framebuffer when the size is zero
		return;
	}

	// Update the OpenGL viewport to match the new size
	glViewport(0, 0, width, height);

	// Bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Resize the color attachment texture
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Resize the renderbuffer for depth and stencil
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	// Check if the framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is not complete!" << std::endl;

	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
