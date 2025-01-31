#include "pch.h"
#include "GraphicsSystem.h"
#include "../Physics/PhysicsSystem.h"
#include <utility>
#include "../Input/Input.h"

#include "FontSystem.h"
#include "AssetManager/AssetManager.h"
#include "ResourceManager/ResourceManager.h"
#include "Windows/WindowManager.h"

#include "Input/Input.h"
#include "EngineCore.h"


bool GraphicsSystem::debug = false;

bool GraphicsSystem::glewInitialized = false;

bool GraphicsSystem::D2 = false;
bool GraphicsSystem::D3 = false;
bool GraphicsSystem::lightOn = false;

CameraComponent GraphicsSystem::camera;
CameraComponent camera_render;

struct light_info {
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
};
std::vector <light_info> lights_infos;


glm::vec3 GraphicsSystem::lightPos = glm::vec3(-3.f, 2.0f, 10.0f);

//int GraphicsSystem::set_Texture_ = 0;
//std::vector<Model2D> models;
ShaderParams shdrParam;

float deltaTime = 0.0f;  // Time between current frame and previous frame
auto previousTime = std::chrono::high_resolution_clock::now();  // Initialize previous time

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



	std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;

	InitializePickingFramebuffer(g_WindowX, g_WindowY);

	// load shaders and models
	g_AssetManager.LoadAll();
	g_ResourceManager.LoadAll();


	AddModel_2D();
	std::cout << "uhee\n\n\n\n\n\n";

	AddEntireModel3D("../BoofWoof/Assets/Objects");
//	AddModel_3D("../BoofWoof/Assets/Objects/Fireplace.obj");
	//fontSystem.init();

	shdrParam.Color = glm::vec3(1.0f, 1.0f, 1.0f);

	// Initialize camera
	camera = CameraComponent(glm::vec3(0.f, 2.f, 10.f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, false);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//fontSystem.init_font();


}





void GraphicsSystem::UpdateLoop() {


	//if (g_Input.IsActionPressed(ActionType::Jump)) {
	//	std::cout << "Jump\n";
	//}

	// Get the current time
	auto currentTime = std::chrono::high_resolution_clock::now();

	// Calculate delta time in seconds
	deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();

	// Update previous time to the current time
	previousTime = currentTime;


	// Bind the framebuffer for rendering
	if (editorMode == true)
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear framebuffer



	


	// Setup camera and projection matrix
	//glm::mat4 view_ = camera.GetViewMatrix();
	//glm::mat4 projection = 

	//g_AssetManager.GetShader("Shader3D").Use();


	// Loop through all entities and render them

	//check camera active
	camera_render = camera;
	for (auto& entity : g_Coordinator.GetAliveEntitiesSet())
	{
		if (g_Coordinator.HaveComponent<CameraComponent>(entity)) {
			auto& cameraComp = g_Coordinator.GetComponent<CameraComponent>(entity);
			if (cameraComp.GetCameraActive()) {
				camera_render = cameraComp;
			}
		}
	}

	lights_infos.clear();
	for (auto& entity : g_Coordinator.GetAliveEntitiesSet())
	{
		if (g_Coordinator.HaveComponent<LightComponent>(entity))
		{
			if (g_Coordinator.HaveComponent<TransformComponent>(entity))
			{
				auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
				light_info light_info_;
				light_info_.position = transformComp.GetPosition();
				light_info_.intensity = g_Coordinator.GetComponent<LightComponent>(entity).getIntensity();
				light_info_.color = g_Coordinator.GetComponent<LightComponent>(entity).getColor();
	
				lights_infos.push_back(light_info_);
	
			}
	
		}
	}
	
	shdrParam.View = camera_render.GetViewMatrix();
	shdrParam.Projection = glm::perspective(glm::radians(45.0f), (float)g_WindowX / (float)g_WindowY, 0.1f, 100.0f);

	glm::mat4 view_ = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)g_WindowX / (float)g_WindowY, 0.1f, 100.0f);
	auto allEntities = g_Coordinator.GetAliveEntitiesSet();
	for (auto& entity : allEntities)
	{/*
		if (g_Coordinator.HaveComponent<CameraComponent>(entity)) {
			auto& cameraComp = g_Coordinator.GetComponent<CameraComponent>(entity);
			if (cameraComp.GetCameraActive()) {
				camera_render = cameraComp;
				std::cout << "Camera Active\n";
			}
		}*/


		if (!g_Coordinator.HaveComponent<TransformComponent>(entity))
		{
			continue;
		}

		auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);

		// Get the TransformSystem instance
		std::shared_ptr<TransformSystem> transformSystem = g_Coordinator.GetSystem<TransformSystem>();

		// Retrieve the world matrix for the current entity
		glm::mat4 worldMatrix = transformSystem->GetWorldMatrix(entity);

		shdrParam.WorldMatrix = worldMatrix; // Use the world matrix for rendering


		if (g_Coordinator.HaveComponent<ParticleComponent>(entity))
		{
			auto& particleComp = g_Coordinator.GetComponent<ParticleComponent>(entity);
			if (!particleComp.getInitFlag()) {
				particleComp.init();
				particleComp.setInitFlag(true);
			}
			g_AssetManager.GetShader("instanced").Use();
			g_AssetManager.GetShader("instanced").SetUniform("view", shdrParam.View);
			g_AssetManager.GetShader("instanced").SetUniform("projection", shdrParam.Projection);
			glPointSize(particleComp.getParticleSize());
			g_AssetManager.GetShader("instanced").SetUniform("particleColor", particleComp.getParticleColor());
			shdrParam.WorldMatrix = transformComp.GetWorldMatrix();
			g_AssetManager.GetShader("instanced").SetUniform("vertexTransform", shdrParam.WorldMatrix);
			//SetShaderUniforms(g_AssetManager.GetShader("instanced"), shdrParam);
			particleComp.update(static_cast<float>(g_Core->m_DeltaTime));
			particleComp.draw();
			g_AssetManager.GetShader("instanced").UnUse();

		}

		if (!g_Coordinator.HaveComponent<GraphicsComponent>(entity))
		{
			continue;
		}

		auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);

		//auto& material = graphicsComp.material;

		auto& ShaderName = "Shader3D";

#ifdef _DEBUG
//		std::cout << "ShaderName: " << material.GetShaderName() << '\n';

#endif

		g_AssetManager.GetShader(ShaderName).Use();

		//if (!g_ResourceManager.hasModel(graphicsComp.getModelName()))
		//{
		//	/* We do not need these anymore */
		//
		//
		//	// g_ResourceManager.SETModel(&g_ResourceManager.ModelMap["Bed2"]);
		//	// std::cout << "Model is null" << std::endl;
		//	//graphicsComp.setModelName("cubeModel");
		//	//graphicsComp.SetModel(&g_AssetManager.ModelMap["Square"]);
		//	continue;
		//}

		if (graphicsComp.getModel() == nullptr)
		{
			//std::cout << "ENTER\n";
			//graphicsComp.SetModel(&g_ResourceManager.ModelMap["Fireplace"]);
			continue;
		}


		if (ShaderName == "Shader3D")
		{

			// START OF 3D
			if (graphicsComp.getFollowCamera()) {
				SetShaderUniforms(g_AssetManager.GetShader(ShaderName), shdrParam);
			}
			else {
				g_AssetManager.GetShader(ShaderName).SetUniform("vertexTransform", shdrParam.WorldMatrix);
				g_AssetManager.GetShader(ShaderName).SetUniform("view", glm::mat4(1.0f));
				g_AssetManager.GetShader(ShaderName).SetUniform("projection", glm::mat4(1.0f));
			
			}
			
			g_AssetManager.GetShader(ShaderName).SetUniform("objectColor", glm::vec3{1.0f});


			for (int i = 0; i < lights_infos.size(); i++)
			{
				std::string lightPosStr = "lights[" + std::to_string(i) + "].position";
				g_AssetManager.GetShader(ShaderName).SetUniform(lightPosStr.c_str(), lights_infos[i].position);
				std::string lightIntensityStr = "lights[" + std::to_string(i) + "].intensity";
				g_AssetManager.GetShader(ShaderName).SetUniform(lightIntensityStr.c_str(), lights_infos[i].intensity);
				std::string lightColorStr = "lights[" + std::to_string(i) + "].color";
				g_AssetManager.GetShader(ShaderName).SetUniform(lightColorStr.c_str(), lights_infos[i].color);
			}



			/*g_AssetManager.GetShader(ShaderName).SetUniform("lights[0].position", lightPos);
			g_AssetManager.GetShader(ShaderName).SetUniform("lights[1].position", glm::vec3(0.0f, 0.0f, 0.0f));*/
			g_AssetManager.GetShader(ShaderName).SetUniform("numLights", static_cast<int>(lights_infos.size()));
			g_AssetManager.GetShader(ShaderName).SetUniform("viewPos", camera_render.Position);
			g_AssetManager.GetShader(ShaderName).SetUniform("lightOn", lightOn);



			graphicsComp.getModel()->Draw(g_AssetManager.GetShader(ShaderName));

			//g_ResourceManager.getModel(graphicsComp.getModelName())->Draw(g_AssetManager.GetShader(ShaderName));
//			g_ResourceManager.getModel()->Draw(g_AssetManager.GetShader(ShaderName));

		}
		else if (ShaderName == "Shader2D")
		{


			// Set shader uniforms based on camera following
			if (graphicsComp.getFollowCamera()) {
				SetShaderUniforms(g_AssetManager.GetShader(ShaderName), shdrParam);
			}
			else {
				g_AssetManager.GetShader(ShaderName).SetUniform("vertexTransform", shdrParam.WorldMatrix);
				g_AssetManager.GetShader(ShaderName).SetUniform("view", glm::mat4(1.0f));
				g_AssetManager.GetShader(ShaderName).SetUniform("projection", glm::mat4(1.0f));
			}

			// Bind texture
//		if (graphicsComp.getTextureNumber() == 0) {
//			glBindTextureUnit(6, 0); // No texture
//		}
//		else {
//			glBindTextureUnit(6, graphicsComp.getTexture(0)); // Texture with transparency
//		}


			// Set texture uniform before drawing
			g_AssetManager.GetShader(ShaderName).SetUniform("uTex2d", 6);




			// Draw the model
			g_ResourceManager.getModel(graphicsComp.getModelName())->Draw2D(g_AssetManager.GetShader(ShaderName));




		}
	//	else if (ShaderName == "Material")
	//	{
	//
	//		
	//		SetShaderUniforms(g_AssetManager.GetShader("Material"), shdrParam);
	//		g_AssetManager.GetShader("Material").SetUniform("inputColor", graphicsComp.material.GetColor());
	//
	//		g_AssetManager.GetShader("Material").SetUniform("inputLight", lightPos);
	//
	//		g_AssetManager.GetShader("Material").SetUniform("viewPos", camera.GetViewMatrix());
	//		g_AssetManager.GetShader("Material").SetUniform("metallic", graphicsComp.material.GetMetallic());
	//		g_AssetManager.GetShader("Material").SetUniform("smoothness", graphicsComp.material.GetSmoothness());
	//
	//
	//		//std::cout << shader.GetDiffuseID() << '\n';
	//
	//		// std::cout << graphicsComp.material.GetDiffuseID() << '\n';
	//
	//
	//		// Check if a texture is set, and bind it
	//		if (graphicsComp.material.GetDiffuseID() >= 0) { // Assuming textureID is -1 if no texture
	//			glActiveTexture(GL_TEXTURE0);
	//			//		std::cout << shader.GetDiffuseID() << '\n';
	//
	//			glBindTexture(GL_TEXTURE_2D, graphicsComp.material.GetDiffuseID());
	//			g_AssetManager.GetShader("Material").SetUniform("albedoTexture", 0);
	//			g_AssetManager.GetShader("Material").SetUniform("useTexture", true);
	//		}
	//		else {
	//			g_AssetManager.GetShader("Material").SetUniform("useTexture", false);
	//		}
	//
	//
	//
	//
	//
	//	}
	//
		g_AssetManager.GetShader("OutlineAndFont").Use();
			
			if (debug)
			{
				SetShaderUniforms(g_AssetManager.GetShader("OutlineAndFont"), shdrParam);
				if (D2)
				{
					Model squareOutline = SquareModelOutline(glm::vec3(0.0f, 1.0f, 0.0f)); // Outline square (green)
					g_ResourceManager.getModel(graphicsComp.getModelName())->DrawCollisionBox2D(squareOutline);

				}

				if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
					auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
					JPH::Body* body = collisionComp.GetPhysicsBody();

					if (body) {
						// Get the world-space AABB from JoltPhysics
						JPH::AABox aabb = body->GetWorldSpaceBounds();

						// Calculate center and half-extents
						JPH::Vec3 center = (aabb.mMin + aabb.mMax) * 0.5f;
						glm::vec3 glmCenter = glm::vec3(0, 0, 0);

						// Apply offset for visual debugging
						glm::vec3 offset = collisionComp.GetAABBOffset();
						glm::vec3 adjustedCenter = glmCenter + offset;

						if (D3) {
							//g_ResourceManager.getModel(graphicsComp.getModelName())->DrawCollisionBox3D(glmCenter, graphicsComp.boundingBox, glm::vec3(0.0f, 1.0f, 1.0f)); // Green color
							g_ResourceManager.getModel(graphicsComp.getModelName())->DrawCollisionBox3D(adjustedCenter, graphicsComp.boundingBox, glm::vec3(0.0f, 1.0f, 1.0f));
						}
					}
				}
			}

			g_AssetManager.GetShader("OutlineAndFont").UnUse();

		
		
		
		g_AssetManager.GetShader(ShaderName).UnUse();


	}

//	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind the framebuffer to switch back to the default framebuffer

	// 2. Picking Rendering Pass (only when needed)
	if (needsPickingRender) {
		RenderSceneForPicking();
		needsPickingRender = false;
	}
}


void GraphicsSystem::AddEntireModel3D(const std::string& directory)
{
	namespace fs = std::filesystem;

	std::unordered_set<std::string> uniqueNames;

	try {
		for (const auto& entry : fs::directory_iterator(directory)) {
			if (entry.is_regular_file()) {
				std::string filename = entry.path().filename().string();
				size_t dotPos = filename.find('.');
				if (dotPos != std::string::npos) {
					uniqueNames.insert(filename.substr(0, dotPos)); // Extract base name
				}
			}
		}

		// Output unique base names
		for (const auto& name : uniqueNames) {
			AddModel_3D(directory + "/" + name + ".obj");

			std::cout << directory + "/" + name + ".obj" << '\n';
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error accessing directory: " << e.what() << std::endl;
	}


}

void GraphicsSystem::AddModel_3D(std::string const& path)
{
	Model model;
	std::cout << "Loading: " << path << '\n';

	model.loadModel(path, GL_TRIANGLES);

	std::string name = path.substr(path.find_last_of('/') + 1);
	//remove .obj from name
	name = name.substr(0, name.find_last_of('.'));

	g_ResourceManager.ModelMap.insert(std::pair<std::string, Model>(name, model));

	g_ResourceManager.addModelNames(name);

	std::cout << "Loaded: " << path << " with name: " << name << " [Models Reference: " << g_ResourceManager.ModelMap.size() - 1 << "]" << '\n';
}

void GraphicsSystem::Draw(std::vector<GraphicsComponent>& components) {
	// Loop through components and draw them
	for (auto& component : components) {
		DrawObject(component);
	}
}

void GraphicsSystem::DrawObject(GraphicsComponent& component)
{
	(void)component;
	// Draw logic using component data
}

void GraphicsSystem::SetShaderUniforms(OpenGLShader& shader, const ShaderParams& shdrParams)
{
	shader.SetUniform("vertexTransform", shdrParams.WorldMatrix);
	shader.SetUniform("view", shdrParams.View);
	shader.SetUniform("projection", shdrParams.Projection);
}






void GraphicsSystem::AddObject_3D(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec3 color, Model* model)
{
	Object object;
	object.model = model;
	object.position = position;
	object.scale = scale;
	object.rotation = rotation;
	object.color = color;
	//	g_AssetManager.Objects.push_back(object);
}



void GraphicsSystem::AddModel_2D()
{
	Model model;

	model = SquareModel(glm::vec3(0.0f));
	g_ResourceManager.SetModelMap(model.name, model);
	std::cout << "Loaded: " << model.name << " [Models Reference: "
		<< g_ResourceManager.GetModelMap().size() - 1 << "]" << '\n';

	// Create CubeModel and add it to ModelMap
	model = CubeModel(glm::vec3(1.0f));
	g_ResourceManager.SetModelMap(model.name, model);
	std::cout << "Loaded: " << model.name << " [Models Reference: "
		<< g_ResourceManager.GetModelMap().size() - 1 << "]" << '\n';
}




void GraphicsSystem::UpdateViewportSize(int width, int height) {
	if (width == 0 || height == 0) {
		// Avoid updating the framebuffer when the size is zero
		return;
	}


	// Store the new viewport dimensions
	viewportWidth = width;
	viewportHeight = height;

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



	// Update picking framebuffer attachments
	glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);

	// Resize the picking color texture
	glBindTexture(GL_TEXTURE_2D, pickingColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Resize the picking renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, pickingRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	// Check framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Picking framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool GraphicsSystem::DrawMaterialSphere()
{
	//	auto& transformComp = 
	g_AssetManager.GetShader("Shader3D").Use();
	//shdrParam.WorldMatrix = transformComp.GetWorldMatrix();
	SetShaderUniforms(g_AssetManager.GetShader("Shader3D"), shdrParam);
	g_AssetManager.GetShader("Shader3D").SetUniform("objectColor", shdrParam.Color);
	g_AssetManager.GetShader("Shader3D").SetUniform("lightPos", lightPos);
	g_AssetManager.GetShader("Shader3D").SetUniform("viewPos", camera.Position);


	//	 g_ResourceManager.getModel("Square")->Draw(g_AssetManager.GetShader("Material"));

	g_AssetManager.GetShader("Shader3D").UnUse();
	return true;
}

void GraphicsSystem::generateNewFrameBuffer(unsigned int& fbo_, unsigned int& textureColorbuffer_, unsigned int& rbo_, int width, int height) {
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &fbo_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

	// Create the texture for the framebuffer
	glGenTextures(1, &textureColorbuffer_);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer_, 0);

	// Create the renderbuffer for depth and stencil
	glGenRenderbuffers(1, &rbo_);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);

	// Check if the framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}

	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsSystem::clearAllEntityTextures()
{
	auto allEntities = g_Coordinator.GetAliveEntitiesSet();
	for (auto& entity : allEntities)
	{

		if (g_Coordinator.HaveComponent<GraphicsComponent>(entity))
		{
			auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
			for (auto& mesh : g_ResourceManager.getModel(graphicsComp.getModelName())->meshes) {
				g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt = 0;
				mesh.textures.clear();

				/*Texture tex;
				mesh.textures.push_back(tex);*/

			}
		}
	}

	std::cout << "Cleared all entities Textures\n";
};

void GraphicsSystem::InitializePickingFramebuffer(int width, int height)
{
	// Generate framebuffer
	glGenFramebuffers(1, &pickingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);

	// Create texture to store object IDs
	glGenTextures(1, &pickingColorTexture);
	glBindTexture(GL_TEXTURE_2D, pickingColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Important for pixel-perfect picking
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingColorTexture, 0);

	// Create a renderbuffer object for depth attachment
	glGenRenderbuffers(1, &pickingRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, pickingRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingRBO);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: Picking framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsSystem::RenderSceneForPicking() {
	glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);
	glViewport(0, 0, viewportWidth, viewportHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the picking shader
	OpenGLShader& pickingShader = g_AssetManager.GetShader("PickingShader");
	pickingShader.Use();

	// Update shader uniforms
	pickingShader.SetUniform("view", camera.GetViewMatrix());
	pickingShader.SetUniform("projection", glm::perspective(glm::radians(45.0f), (float)g_WindowX / (float)g_WindowY, 0.1f, 100.0f));

	// Render each entity with its unique ID color
	auto allEntities = g_Coordinator.GetAliveEntitiesSet();
	for (auto& entity : allEntities)
	{
		if (g_Coordinator.HaveComponent<TransformComponent>(entity))
		{
			auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
			// Get the TransformSystem instance
			std::shared_ptr<TransformSystem> transformSystem = g_Coordinator.GetSystem<TransformSystem>();
			glm::mat4 worldMatrix = transformSystem->GetWorldMatrix(entity);
			pickingShader.SetUniform("vertexTransform", worldMatrix);

			// Encode the entity ID as a color
			glm::vec3 idColor = EncodeIDToColor(entity);
			pickingShader.SetUniform("objectIDColor", idColor);

			// Retrieve the model (ensure you have error checking)
			if (g_Coordinator.HaveComponent<GraphicsComponent>(entity))
			{
				auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);

				// Render the model without textures or materials
				Model* model = g_ResourceManager.getModel(graphicsComp.getModelName());
				if (model)
				{
					model->DrawForPicking();
				}
			}
		}
	}

	pickingShader.UnUse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::vec3 GraphicsSystem::EncodeIDToColor(Entity id)
{
	unsigned int r = (id & 0x000000FF) >> 0;
	unsigned int g = (id & 0x0000FF00) >> 8;
	unsigned int b = (id & 0x00FF0000) >> 16;

	return glm::vec3(r, g, b) / 255.0f;
}

Entity GraphicsSystem::DecodeColorToID(unsigned char* data)
{
	unsigned int r = data[0];
	unsigned int g = data[1];
	unsigned int b = data[2];

	unsigned int id = r + (g << 8) + (b << 16);

	if (id < MAX_ENTITIES)
	{
		return static_cast<Entity>(id);
	}
	else
	{
		return MAX_ENTITIES; // Invalid ID
	}
}

