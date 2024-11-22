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
bool GraphicsSystem::lightOn = true;

CameraComponent GraphicsSystem::camera;
CameraComponent camera_render;
ParticleComponent Particle_cmp;

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


	InitializePickingFramebuffer(g_WindowX, g_WindowY);

	// load shaders and models
	g_AssetManager.LoadAll();
	g_ResourceManager.LoadAll();


	AddModel_2D();

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
	shdrParam.View = camera_render.GetViewMatrix();
	shdrParam.Projection = glm::perspective(glm::radians(45.0f), (float)g_WindowX / (float)g_WindowY, 0.1f, 100.0f);



	auto allEntities = g_Coordinator.GetAliveEntitiesSet();
	for (auto& entity : allEntities)
	{
		if (g_Coordinator.HaveComponent<CameraComponent>(entity)) {
			auto& cameraComp = g_Coordinator.GetComponent<CameraComponent>(entity);
			if (cameraComp.GetCameraActive()) {
				camera_render = cameraComp;
				break;
			}
		}

		if (!g_Coordinator.HaveComponent<TransformComponent>(entity))
		{
			continue;
		}
		auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);


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
			glPointSize(10.0f);


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

		shdrParam.WorldMatrix = transformComp.GetWorldMatrix();

		g_AssetManager.GetShader("Shader3D").Use();

		if (!g_ResourceManager.hasModel(graphicsComp.getModelName()))
		{
			/* We do not need these anymore */

			// std::cout << "Model is null" << std::endl;
			//graphicsComp.setModelName("cubeModel");
			//graphicsComp.SetModel(&g_AssetManager.ModelMap["Square"]);
			continue;
		}



		// START OF 3D
		if (graphicsComp.getFollowCamera()) {
			SetShaderUniforms(g_AssetManager.GetShader("Shader3D"), shdrParam);
		}else {
			shdrParam.WorldMatrix = transformComp.GetWorldMatrix();
			shdrParam.View = glm::mat4(1.0f);
			shdrParam.Projection = glm::mat4(1.0f);
			SetShaderUniforms(g_AssetManager.GetShader("Shader3D"), shdrParam);
		}
		g_AssetManager.GetShader("Shader3D").SetUniform("objectColor", shdrParam.Color);
		g_AssetManager.GetShader("Shader3D").SetUniform("lightPos", lightPos);
		g_AssetManager.GetShader("Shader3D").SetUniform("viewPos", camera_render.Position);
		g_AssetManager.GetShader("Shader3D").SetUniform("lightOn", lightOn);

		/*std::cout << "entity "<< entity << "\n";
		std::cout << "model text cnt " << g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt << "\n";
		std::cout << "comp tetx cnt "<<graphicsComp.getTextureNumber() << "\n";*/


		//if (graphicsComp.getTextureNumber() == 0) {
		for (auto& mesh : g_ResourceManager.getModel(graphicsComp.getModelName())->meshes) {
			mesh.textures.clear();
		}

		g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt = 0;
		//}


		while (g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt < graphicsComp.getTextureNumber()) {


#ifdef _DEBUG
			//std::cout << g_ResourceManager.getModel(graphicsComp.getModelName())->name << '\n';

			//std::cout << graphicsComp.getModelName() << '\n';

			//std::cout << g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt << '\t' << g_ResourceManager.getModel(graphicsComp.getModelName())->textures_loaded.size() << '\n';
#endif

					// add texture to mesh
			Texture texture_add;

			//if(graphicsComp.getModelName() == "sphere")
			texture_add.id = graphicsComp.getTexture(g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt);
			//else
				//texture_add.id = g_ResourceManager.GetTextureDDS(graphicsComp.getTextureName());

			//std::cout << texture_add.id << "\n";

			if (g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt == 0)
				texture_add.type = "texture_diffuse";
			else if (g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt == 1)
				texture_add.type = "texture_normal";
			else
				texture_add.type = "texture_specular";

			//std::cout << texture_add.type << '\n';

			//std::cout << "mesh size: " << g_ResourceManager.getModel(graphicsComp.getModelName())->meshes.size() << "\n";

			for (auto& mesh : g_ResourceManager.getModel(graphicsComp.getModelName())->meshes) {
				//	std::cout << "texture size before adding: " << mesh.textures.size() << "\n";
					//mesh.textures.clear();
				mesh.textures.push_back(texture_add);
				//	std::cout << "entered\n";
			}

			g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt++;

		}


		//clear all textures
		//for (auto& mesh : g_ResourceManager.getModel(graphicsComp.getModelName())->meshes) {
		////	std::cout << "texture size after clearing: " << mesh.textures.size() << "\n";
		//}
		g_AssetManager.GetShader("Shader3D").SetUniform("textureCount", g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt);
		//std::cout << "out model text cnt " << g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt << "\n";
		//std::cout << "out comp tetx cnt " << graphicsComp.getTextureNumber() << "\n";


		if (debug) 
		{
			//// Debug AABB Drawing
			//if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
			//	auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
			//	JPH::Body* body = collisionComp.GetPhysicsBody();

			//	if (body) {
			//		// Get the world-space AABB from JoltPhysics
			//		JPH::AABox aabb = body->GetWorldSpaceBounds();

			//		// Calculate center and half-extents
			//		JPH::Vec3 center = (aabb.mMin + aabb.mMax) * 0.5f;
			//		JPH::Vec3 halfExtents = (aabb.mMax - aabb.mMin) * 0.5f;

			//		std::cout << transformComp.GetRotation().x << '\t' << transformComp.GetRotation().y << '\t' << transformComp.GetRotation().z << '\n';

			//		// Convert to glm::vec3
			//		glm::vec3 glmCenter = glm::vec3(center.GetX(), center.GetY(), center.GetZ());
			//		glm::vec3 glmHalfExtents = glm::vec3(halfExtents.GetX(), halfExtents.GetY(), halfExtents.GetZ());

			//		std::cout << "Explain to me: " << glmHalfExtents.x << '\t' << glmHalfExtents.y << '\t' << glmHalfExtents.z << "\n";

			//		if (D3)
			//		{
			//			// Draw the AABB using your existing DrawCollisionBox3D function
			//			//g_ResourceManager.getModel("cubeModel")->DrawCollisionBox3D(glmCenter, glmHalfExtents, glm::vec3(0.0f, 1.0f, 0.0f)); // Green color
			//			//g_ResourceManager.getModel(graphicsComp.getModelName())->DrawCollisionBox3D(glmCenter, glmHalfExtents, transformComp.GetRotation(), glm::vec3(0.0f, 1.0f, 0.0f)); // Green color

			//			g_ResourceManager.getModel("cubdeModel")->drawOBB(glmCenter, transformComp.GetRotation() * glm::vec3(1 / 12 * 3.14159265358979323846), glmHalfExtents);
			//		}
			//	}
			//}
			
			// Debug AABB Drawing
			if (g_Coordinator.HaveComponent<CollisionComponent>(entity)) {
				auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
				JPH::Body* body = collisionComp.GetPhysicsBody();
				if (body) {
					// Get unique ID for this body - using GetIndex() to get the uint32
					uint32_t bodyId = body->GetID().GetIndex();

					// Get the world-space AABB from JoltPhysics
					JPH::AABox aabb = body->GetWorldSpaceBounds();

					// Calculate center and half-extents
					JPH::Vec3 center = (aabb.mMin + aabb.mMax) * 0.5f;
					glm::vec3 glmCenter = glm::vec3(center.GetX(), center.GetY(), center.GetZ());

					// If this body's half extents haven't been stored yet, store them
					if (bodyInitialHalfExtents.find(bodyId) == bodyInitialHalfExtents.end()) {
						JPH::Vec3 halfExtents = (aabb.mMax - aabb.mMin) * 0.5f;
						bodyInitialHalfExtents[bodyId] = glm::vec3(halfExtents.GetX(), halfExtents.GetY(), halfExtents.GetZ());
					}

					// Use the stored half extents for this specific body
					glm::vec3& initialHalfExtents = bodyInitialHalfExtents[bodyId];

					std::cout << transformComp.GetRotation().x << '\t' << transformComp.GetRotation().y << '\t' << transformComp.GetRotation().z << '\n';
					std::cout << "Explain to me: " << initialHalfExtents.x << '\t' << initialHalfExtents.y << '\t' << initialHalfExtents.z << "\n";

					if (D3) {
						g_ResourceManager.getModel("cubdeModel")->drawOBB(glmCenter, transformComp.GetRotation(), initialHalfExtents);
					}
				}
			}

			//if (D3) 
			//{
			//	g_ResourceManager.getModel(graphicsComp.getModelName())->DrawLine();
			//}
		}
		/*//skip for now
		for (int i = 0; i < graphicsComp.getTextureNumber(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			static bool show = true;
			if (show) {
				std::cout << "Texture Count: " << graphicsComp.getTextureNumber() <<" with 1st texture number " << graphicsComp.getTexture(i) << std::endl;
				show = false;
			}
			g_AssetManager.GetShader("Shader3D").SetUniform("texture1", i);
			glBindTexture(GL_TEXTURE_2D, graphicsComp.getTexture(i));
		}*/

		//	g_AssetManager.GetShader("Shader3D").SetUniform("texture1", tex1);
		//	g_AssetManager.GetShader("Shader3D").SetUniform("texture2", tex2);
//				g_AssetManager.GetShader("OutlineAndFont").SetUniform("objectColor", glm::vec3(1.0f,1.0f,1.0f));

				//graphicsComp.getModel()->Draw(g_AssetManager.GetShader("Shader3D"));
				//std::cout << "Drawing entity: " << entity << '\n';
		g_ResourceManager.getModel(graphicsComp.getModelName())->Draw(g_AssetManager.GetShader("Shader3D"));
		//				g_AssetManager.ModelMap[graphicsComp.getModelName()].Draw(g_AssetManager.GetShader("Shader3D"));
						//graphicsComp.getModel()->DrawLine(g_AssetManager.GetShader("OutlineAndFont"));

		g_AssetManager.GetShader("Shader3D").UnUse();




		//Model outline = ModelOutline3D(,glm::vec3(0.0f, 1.0f, 0.0f));
		// START OF 3D BOX WIREFRAME MODE

//				g_AssetManager.GetShader("OutlineAndFont").Use();

				// END OF 3D


		g_AssetManager.GetShader("Shader2D").Use();

		SetShaderUniforms(g_AssetManager.GetShader("Shader2D"), shdrParam);
		//shader.Use();



		//tex = g_ResourceManager.GetTextureDDS("Sadge");


		// SET ALL SAME TEX...
//				set_Texture_T = GraphicsSystem::set_Texture_;



		g_AssetManager.GetShader("Shader2D").SetUniform("uTex2d", 6);
		//shader.SetUniform("")



		if (graphicsComp.getTextureNumber() == 0)
		{
			glBindTextureUnit(6, 0);
			//std::cout << "its blank\n";
		}
		else
			glBindTextureUnit(6, graphicsComp.getTexture(0));
		//glBindTextureUnit(6, set_Texture_T);

		if (debug)
		{
			if (D2)
			{
				Model squareOutline = SquareModelOutline(glm::vec3(0.0f, 1.0f, 0.0f)); // Outline square (green)
				g_ResourceManager.getModel(graphicsComp.getModelName())->DrawCollisionBox2D(squareOutline);
				g_AssetManager.GetShader("OutlineAndFont").Use();
				SetShaderUniforms(g_AssetManager.GetShader("OutlineAndFont"), shdrParam);

				g_AssetManager.GetShader("OutlineAndFont").UnUse();
			}
		}

		//graphicsComp.getModel()->Draw2D(g_AssetManager.GetShader("Shader2D"));
		//g_AssetManager.ModelMap[graphicsComp.getModelName()].Draw2D(g_AssetManager.GetShader("Shader2D"));
//				g_ResourceManager.ModelMap[graphicsComp.getModelName()].Draw2D(g_AssetManager.GetShader("Shader2D"));

		g_ResourceManager.getModel(graphicsComp.getModelName())->Draw2D(g_AssetManager.GetShader("Shader2D"));

		g_AssetManager.GetShader("Shader2D").UnUse();

	}



	//glm::mat4 fontprojection = glm::ortho(0.0f, static_cast<float>(g_WindowX), 0.0f, static_cast<float>(g_WindowY));
	//g_AssetManager.GetShader("Font").SetUniform("projection", fontprojection);

	//fontSystem.RenderText(g_AssetManager.GetShader("Font"), "Hello, World!", 0.0f, 0.0f, 0.001f, glm::vec3(1.f, 0.8f, 0.2f));
	//fontSystem.render_text(g_AssetManager.GetShader("Font"), "Hello, World!", 0.0f, 0.0f, 0.001f, glm::vec3(1.f, 0.8f, 0.2f));


	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind the framebuffer to switch back to the default framebuffer



	// 2. Picking Rendering Pass (only when needed)
	if (needsPickingRender) {
		RenderSceneForPicking();
		needsPickingRender = false;
	}
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

void GraphicsSystem::generateNewFrameBuffer(unsigned int& fbo, unsigned int& textureColorbuffer, unsigned int& rbo, int width, int height) {
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create the texture for the framebuffer
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// Create the renderbuffer for depth and stencil
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

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
			glm::mat4 worldMatrix = transformComp.GetWorldMatrix();
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

