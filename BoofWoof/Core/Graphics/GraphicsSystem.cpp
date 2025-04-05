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

#include "../Core/AssetManager/FilePaths.h"

#include "Animation/Animation.h"
#include "Animation/Animator.h"

#include <glm/gtx/string_cast.hpp >




//                       _oo0oo_
//                      o8888888o
//                      88" . "88
//                      (| -_- |)
//                      0\  =  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  -  /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




bool GraphicsSystem::debug = false;

bool GraphicsSystem::glewInitialized = false;

bool GraphicsSystem::D2 = false;
bool GraphicsSystem::D3 = false;
bool GraphicsSystem::lightOn = false;
float GraphicsSystem::gammaValue = 2.2f;

CameraComponent GraphicsSystem::camera;
CameraComponent camera_render;


std::vector<DebugLine> GraphicsSystem::debugLines = {};
unsigned int GraphicsSystem::debugLineVAO = 0;
unsigned int GraphicsSystem::debugLineVBO = 0;

const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
unsigned int depthMapFBO;
unsigned int depthMap_texture;


struct light_info {
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	glm::mat4 lightSpaceMatrix;
	bool haveshadow;
	float range;
};
std::vector <light_info> lights_infos;


glm::vec3 GraphicsSystem::lightPos = glm::vec3(-3.f, 2.0f, 10.0f);

//int GraphicsSystem::set_Texture_ = 0;
//std::vector<Model2D> models;
ShaderParams shdrParam;


//Model corgModel;
//AnimationT danceAnimationZ;
//Animator animatorZ;

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


	//corgModel = Model("../BoofWoof/Assets/Animations/corgi.fbx", false);
	//danceAnimationZ = AnimationT("../BoofWoof/Assets/Animations/corgi.fbx", &corgModel);
	//animatorZ = Animator(&danceAnimationZ);


	AddModel_2D();
	std::cout << "uhee\n\n\n\n\n\n";

	AddEntireModel3D(FILEPATH_ASSET_OBJECTS);
	//	AddModel_3D("../BoofWoof/Assets/Objects/Fireplace.obj");
		//fontSystem.init();

	std::cout << "its hjere\n";

	AddAllAnimations(FILEPATH_ASSET_ANIMATIONS);


	//TestAnimationAdd("corgi_walk", "../BoofWoof/Assets/Animations/corgi_walk.fbx");
	//TestAnimationAdd("corgi", "../BoofWoof/Assets/Animations/corgi.fbx");
	//TestAnimationAdd("vampire", "../BoofWoof/Assets/Animations/dancing_vampire.dae");


	shdrParam.Color = glm::vec3(1.0f, 1.0f, 1.0f);

	// Initialize camera
	camera = CameraComponent(glm::vec3(0.f, 2.f, 10.f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, false);

	// depthmap Fbo
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture

	// create depth texture
	glGenTextures(1, &depthMap_texture);
	glBindTexture(GL_TEXTURE_2D, depthMap_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}





void GraphicsSystem::UpdateLoop() {

	glDepthRange(0.1, 1.0);

	//if (g_Input.IsActionPressed(ActionType::Jump)) {
	//	std::cout << "Jump\n";
	//}

	// Get the current time
	auto currentTime = std::chrono::high_resolution_clock::now();

	// Calculate delta time in seconds
	deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();

	// Update previous time to the current time
	previousTime = currentTime;

	if (editorMode == true)
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	else
		glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



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



	shdrParam.View = camera_render.GetViewMatrix();
	shdrParam.Projection = glm::perspective(glm::radians(45.0f), (g_WindowY > 0) ? ((float)g_WindowX / (float)g_WindowY) : 1, 0.1f, 100.0f);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	lights_infos.clear();
	for (auto& entity : g_Coordinator.GetAliveEntitiesSet())
	{
		if (g_Coordinator.HaveComponent<LightComponent>(entity))
		{
			if (g_Coordinator.HaveComponent<TransformComponent>(entity))
			{
				auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
				auto& lightComp = g_Coordinator.GetComponent<LightComponent>(entity);
				light_info light_info_;
				light_info_.position = transformComp.GetPosition();
				light_info_.intensity = lightComp.getIntensity();
				light_info_.color = lightComp.getColor();
				light_info_.haveshadow = lightComp.getShadow();


				glm::mat4 lightProjection, lightView;
				glm::mat4 lightSpaceMatrix;
				float near_plane = 1.0f, far_plane = 17.5f;
				lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
				lightView = glm::lookAt(light_info_.position, (light_info_.position + lightComp.getDirection()), glm::vec3(0.0f, 1.0f, 0.0f));
				lightSpaceMatrix = lightProjection * lightView;
				light_info_.lightSpaceMatrix = lightSpaceMatrix;
				light_info_.range = lightComp.getRange();
				lights_infos.push_back(light_info_);
				if (light_info_.haveshadow) {
					g_AssetManager.GetShader("Direction_light_Space").Use();
					g_AssetManager.GetShader("Direction_light_Space").SetUniform("lightSpaceMatrix", lightSpaceMatrix);
					RenderScence(g_AssetManager.GetShader("Direction_light_Space"));
					g_AssetManager.GetShader("Direction_light_Space").UnUse();
				}

			}

		}
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*if (editorMode == true) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		glViewport(viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y);
	}
	else {
		glViewport(0, 0, g_WindowX, g_WindowY);
	}*/
	if (editorMode == true)
		glViewport(0, 0, viewportWidth, viewportHeight);
	else
		glViewport(0, 0, g_WindowX, g_WindowY);

	// Bind the framebuffer for rendering
	if (editorMode == true)

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear framebuffer



	auto allEntities = g_Coordinator.GetAliveEntitiesSet();
	for (auto& entity : allEntities)
	{

		/*
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
				particleComp.init(&g_ResourceManager.getModel(particleComp.getParticleModelname())->meshes[0]);
				particleComp.setInitFlag(true);
			}
			OpenGLShader particleShader;
			switch (particleComp.getParticleType())
			{
			case ParticleType::POINT:
				particleShader = g_AssetManager.GetShader("instanced_point");
				break;
			case ParticleType::TEXTURED:
				particleShader = g_AssetManager.GetShader("instanced_texture");
				break;
			case ParticleType::TEXTURED_3D:
				particleShader = g_AssetManager.GetShader("instanced_3Dobj");
				break;
			}
			particleShader.Use();
			particleShader.SetUniform("view", shdrParam.View);
			particleShader.SetUniform("projection", shdrParam.Projection);
			glPointSize(particleComp.getParticleSize());
			if (particleComp.getParticleType() == ParticleType::POINT)
				particleShader.SetUniform("particleColor", particleComp.getParticleColor());
			particleShader.SetUniform("opacity", particleComp.getOpacity());
			shdrParam.WorldMatrix = transformComp.GetWorldMatrix();
			particleShader.SetUniform("vertexTransform", shdrParam.WorldMatrix);
			particleShader.SetUniform("gammaValue", gammaValue);

			particleComp.update_textured(static_cast<float>(g_Core->m_DeltaTime));

			GLuint text{};
			GLuint tex_loc{};

			switch (particleComp.getParticleType())
			{
			case ParticleType::POINT:
				particleComp.draw();
				break;
			case ParticleType::TEXTURED:
				shdrParam.WorldMatrix = transformComp.GetWorldMatrix_withoutRotate();
				particleShader.SetUniform("vertexTransform", shdrParam.WorldMatrix);
				text = g_ResourceManager.GetTextureDDS(particleComp.getParticleTexturename());
				glBindTextureUnit(6, text);
				glBindTexture(GL_TEXTURE_2D, text);
				tex_loc = glGetUniformLocation(particleShader.GetHandle(), "uTex2d");
				glUniform1i(tex_loc, 6);
				particleComp.draw();
				break;
			case ParticleType::TEXTURED_3D:

				text = g_ResourceManager.GetTextureDDS(particleComp.getParticleTexturename());
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, text);
				tex_loc = glGetUniformLocation(particleShader.GetHandle(), "uTex2d");
				glUniform1i(tex_loc, 0);
				particleComp.draw();
				glActiveTexture(GL_TEXTURE0);
				break;
			}
			particleShader.UnUse();

		}

		if (!g_Coordinator.HaveComponent<GraphicsComponent>(entity))
		{
			continue;
		}

		auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);

		auto& material = graphicsComp.material;

		graphicsComp.deltaTime = deltaTime;


		//// Ensure the animation and animator exist before rendering
		//if (g_ResourceManager.AnimatorMap.find("corgi_walk") == g_ResourceManager.AnimatorMap.end()) {
		//	auto animIt = g_ResourceManager.AnimationMap.find("corgi_walk");
		//	if (animIt != g_ResourceManager.AnimationMap.end()) {
		//		Animation* animation = animIt->second; // Fetch animation from AnimationMap
		//		g_ResourceManager.AnimatorMap["corgi_walk"] = new Animator(animation); // Create new animator
		//		std::cout << "Animator for corgi_walk created.\n";
		//	}
		//	else {
		//		std::cerr << "Error: Animation 'corgi_walk' not found in AnimationMap.\n";
		//	}
		//}




//		auto& ShaderName = material.GetShaderNameRef();


	//	std::cout << ShaderName << '\n';
		auto& ShaderName = "Direction_obj_render";

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

		g_AssetManager.GetShader(ShaderName).SetUniform("gammaValue", gammaValue);

		//	std::cout << material.GetGammaValue();

		if (graphicsComp.getModel() == nullptr)
		{
			//std::cout << "ENTER\n";
			//graphicsComp.SetModel(&g_ResourceManager.ModelMap["Fireplace"]);
			continue;
		}

		//	std::cout <<"shader: \t" << material.GetShaderIndex() << '\t' << material.GetShaderName() << '\n';

		if (g_Coordinator.HaveComponent<AnimationComponent>(entity))
		{
			auto& animationComp = g_Coordinator.GetComponent<AnimationComponent>(entity);
			//std::cout << "it has animationComp.\n";


			
				if (g_ResourceManager.AnimationMap.find(graphicsComp.getModelName()) != g_ResourceManager.AnimationMap.end()) {


//					animationComp.m_DeltaTime = g_ResourceManager.AnimatorMap[graphicsComp.getModelName()]->GetCurrTime();
//					animationComp.m_Duration = g_ResourceManager.AnimationMap[graphicsComp.getModelName()]->GetDuration();

					if (material.GetShaderName() == "Animation") {
						// Bind and use the animation shader
						g_AssetManager.GetShader("Animation").Use();


						


						if (!graphicsComp.pauseAnimation)
						{
							if (g_Coordinator.GetEntityId(entity) == 541)
							{
								std::cout << "it did entered here \n";
								g_ResourceManager.AnimatorMap[graphicsComp.getModelName()]->UpdateAnimation(graphicsComp.deltaTime / 3);
							}
							if (g_Coordinator.GetEntityId(entity) == 543)
							{
								g_ResourceManager.AnimatorMap[graphicsComp.getModelName()]->UpdateAnimation(graphicsComp.deltaTime / 2);
							}
							else
								g_ResourceManager.AnimatorMap[graphicsComp.getModelName()]->UpdateAnimation(graphicsComp.deltaTime);
							
						}
						else
							g_ResourceManager.AnimatorMap[graphicsComp.getModelName()]->SetAnimationTime(graphicsComp.GetAnimationTime());




						g_AssetManager.GetShader("Animation").SetUniform("view", shdrParam.View);
						g_AssetManager.GetShader("Animation").SetUniform("projection", shdrParam.Projection);


						auto transforms = g_ResourceManager.AnimatorMap[graphicsComp.getModelName()]->GetFinalBoneMatrices();
						for (int i = 0; i < transforms.size(); ++i)
						{
							std::string lel = "finalBonesMatrices[" + std::to_string(i) + "]";
							g_AssetManager.GetShader("Animation").SetUniform(lel.c_str(), transforms[i]);
						}


						g_AssetManager.GetShader("Animation").SetUniform("model", transformComp.GetWorldMatrix());
						graphicsComp.getModel()->Draw(g_AssetManager.GetShader("Animation"));


						// Unbind the shader after rendering
						g_AssetManager.GetShader("Animation").UnUse();

					}
				}
			




		}
		else
		{







			//if (ShaderName == "Shader3D")
			if (strcmp(ShaderName, "Direction_obj_render") == 0)
			{
				//			std::cout << graphicsComp.getFollowCamera() << '\n';
							// START OF 3D
				if (graphicsComp.getFollowCamera()) {
					SetShaderUniforms(g_AssetManager.GetShader(ShaderName), shdrParam);
				}
				else {
					g_AssetManager.GetShader(ShaderName).SetUniform("vertexTransform", shdrParam.WorldMatrix);
					g_AssetManager.GetShader(ShaderName).SetUniform("view", glm::mat4(1.0f));
					g_AssetManager.GetShader(ShaderName).SetUniform("projection", glm::mat4(1.0f));

				}

				g_AssetManager.GetShader(ShaderName).SetUniform("objectColor", glm::vec3{ 1.0f });
				g_AssetManager.GetShader(ShaderName).SetUniform("brightness", brightness);

				glm::mat4 lightmtx(1.0f);

				for (int i = 0; i < lights_infos.size(); i++)
				{
					std::string lightPosStr = "lights[" + std::to_string(i) + "].position";
					g_AssetManager.GetShader(ShaderName).SetUniform(lightPosStr.c_str(), lights_infos[i].position);
					std::string lightIntensityStr = "lights[" + std::to_string(i) + "].intensity";
					g_AssetManager.GetShader(ShaderName).SetUniform(lightIntensityStr.c_str(), lights_infos[i].intensity);
					std::string lightColorStr = "lights[" + std::to_string(i) + "].color";
					g_AssetManager.GetShader(ShaderName).SetUniform(lightColorStr.c_str(), lights_infos[i].color);
					std::string lightShadowStr = "lights[" + std::to_string(i) + "].haveshadow";
					g_AssetManager.GetShader(ShaderName).SetUniform(lightShadowStr.c_str(), lights_infos[i].haveshadow);
					if (lights_infos[i].haveshadow)
						lightmtx = lights_infos[i].lightSpaceMatrix;
					std::string lightRangeStr = "lights[" + std::to_string(i) + "].range";

					g_AssetManager.GetShader(ShaderName).SetUniform(lightRangeStr.c_str(), lights_infos[i].range);
				}
				g_AssetManager.GetShader(ShaderName).SetUniform("lightSpaceMatrix", lightmtx);



				if (graphicsComp.getModelName() == "cubeModel")
				{

					g_AssetManager.GetShader("Shader2D").SetUniform("opacity", 1.0f);

					//	std::cout << material.GetMaterialRed() << '\t' << material.GetMaterialGreen() << '\t' << material.GetMaterialBlue() << '\n';

					g_AssetManager.GetShader("Shader2D").SetUniform("inputColor", material.GetMaterialRGB());
					// Set texture uniform before drawing
					g_AssetManager.GetShader("Shader2D").SetUniform("uTex2d", 6);

					g_ResourceManager.getModel(graphicsComp.getModelName())->Draw2D(g_AssetManager.GetShader("Shader2D"));

					continue;
				}


				// Bind the depth texture to texture unit 1 and tell the shader to use unit 1 for the shadow map.
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, depthMap_texture);
				g_AssetManager.GetShader(ShaderName).SetUniform("shadowMap", 1);
				/*g_AssetManager.GetShader(ShaderName).SetUniform("lights[0].position", lightPos);
				g_AssetManager.GetShader(ShaderName).SetUniform("lights[1].position", glm::vec3(0.0f, 0.0f, 0.0f));*/
				g_AssetManager.GetShader(ShaderName).SetUniform("numLights", static_cast<int>(lights_infos.size()));
				//g_AssetManager.GetShader(ShaderName).SetUniform("viewPos", camera_render.Position);
				g_AssetManager.GetShader(ShaderName).SetUniform("lightOn", lightOn);
				//g_AssetManager.GetShader(ShaderName).SetUniform("inputColor", glm::vec4(1.0f,1.0f,1.0f,1.0f));

				//g_AssetManager.GetShader(ShaderName).SetUniform("roughness", 1.0f);

				graphicsComp.getModel()->Draw(g_AssetManager.GetShader(ShaderName));

				//g_ResourceManager.getModel(graphicsComp.getModelName())->Draw(g_AssetManager.GetShader(ShaderName));
	//			g_ResourceManager.getModel()->Draw(g_AssetManager.GetShader(ShaderName));

			}
			//else if (ShaderName == "Shader2D")
			else if (strcmp(ShaderName, "Shader2D") == 0)
			{
				OpenGLShader& shader = g_AssetManager.GetShader(ShaderName);


				// Set shader uniforms based on camera following
				if (graphicsComp.getFollowCamera()) {
					SetShaderUniforms(g_AssetManager.GetShader(ShaderName), shdrParam);
				}
				else {
					shader.SetUniform("vertexTransform", shdrParam.WorldMatrix);
					shader.SetUniform("view", glm::mat4(1.0f));
					shader.SetUniform("projection", glm::mat4(1.0f));
				}

				// Set Texture Uniform once before rendering
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
		}
		if (debug && (D2 || D3))
		{
			OpenGLShader& shader = g_AssetManager.GetShader("Debug");
			shader.Use();

			SetShaderUniforms(shader, shdrParam); // Ensure color is set correctly

			auto model = g_ResourceManager.getModel(graphicsComp.getModelName());

			if (D2)
			{
				model->DrawCollisionBox2D(SquareModelOutline(glm::vec3(0.0f, 1.0f, 0.0f))); // Green for 2D
			}
			else if (g_Coordinator.HaveComponent<CollisionComponent>(entity))
			{
				auto& collisionComp = g_Coordinator.GetComponent<CollisionComponent>(entity);
				JPH::Body* body = collisionComp.GetPhysicsBody();

				if (body)
				{
					// Get the world-space AABB from JoltPhysics
					JPH::AABox aabb = body->GetWorldSpaceBounds();

					// Calculate center and half-extents
					JPH::Vec3 center = (aabb.mMin + aabb.mMax) * 0.5f;

					// Apply offset for visual debugging
					glm::vec3 offset = collisionComp.GetAABBOffset() * 2.0f;

					// Define debug color (cyan: 0.0f, 1.0f, 1.0f)
					glm::vec3 debugColor = glm::vec3(0.0f, 1.0f, 1.0f);

					// Ensure shader receives the color
					//shader.SetUniform("objectColor", debugColor);

					model->DrawCollisionBox3D(offset, graphicsComp.boundingBox, debugColor);

				}
			}



			shader.UnUse();
		}






		g_AssetManager.GetShader(ShaderName).UnUse();


	}

	if (editorMode == true)
	{
		RenderLightPos();
		/*g_AssetManager.GetShader("Direction_light_debug").Use();
		//g_AssetManager.GetShader("Direction_light_debug").SetUniform("near_plane", 1.0f);
		//g_AssetManager.GetShader("Direction_light_debug").SetUniform("far_plane", 7.5f);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap_texture);
		g_AssetManager.GetShader("Direction_light_debug").SetUniform("depthMap", 1);
		g_ResourceManager.getModel("cubeModel")->DrawForPicking();
		g_AssetManager.GetShader("Direction_light_debug").UnUse();*/

	}

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind the framebuffer to switch back to the default framebuffer

		// 2. Picking Rendering Pass (only when needed)
	if (needsPickingRender) {
		RenderSceneForPicking();
		needsPickingRender = false;
	}


	glDepthRange(0.0, 1.0);
	glDisable(GL_DEPTH_TEST);
	RenderDebugLines();
	glEnable(GL_DEPTH_TEST);
}

void GraphicsSystem::Clean()
{

	//for (auto anim : g_ResourceManager.AnimationVec)
	//	delete anim; // Clean up animations
	//
	//for (auto animTor : g_ResourceManager.AnimatorVec)
	//	delete animTor; // Clean up animators

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
	auto model = std::make_unique<Model>();
	std::cout << "Loading: " << path << '\n';

	model->loadModel(path);//, GL_TRIANGLES);

	std::string name = path.substr(path.find_last_of('/') + 1);
	//remove .obj from name
	name = name.substr(0, name.find_last_of('.'));

	//	g_ResourceManager.ModelMap.insert(std::pair<std::string, Model>(name, model));

	g_ResourceManager.ModelMap[name] = std::move(model);

	g_ResourceManager.addModelNames(name);

	std::cout << "Loaded: " << path << " with name: " << name << " [Models Reference: " << g_ResourceManager.ModelMap.size() - 1 << "]" << '\n';
}


//void GraphicsSystem::TestAnimationAdd(const std::string& path) {
//	// Step 1: Load model (needed for animation)
//	Model animModel(path, false);
//
//	std::string name = "corgi_walk";
//
//	// Step 2: Create AnimationT object and store it
////	Animation* corgiWalk = new AnimationT(path, &animModel);
//
//	AnimationT corgiWalk(path, &animModel);
//	Animator animator(&corgiWalk);
//
//	g_ResourceManager.AnimationMap.insert(std::pair<std::string, AnimationT>(name, corgiWalk));
//
//	g_ResourceManager.AnimatorMap.insert(std::pair<std::string, Animator>(name, animator));
//	// Step 3: Store the model
//	g_ResourceManager.ModelMap.insert(std::pair<std::string, Model>(name, animModel));
//	g_ResourceManager.addModelNames(name);
//
//
//	std::cout << "Animation and Animator added successfully!\n";
//}

void GraphicsSystem::AddAllAnimations(const std::string& filepath)
{

	namespace fs = std::filesystem;

	std::unordered_set<std::string> uniqueNames;

	try {
		for (const auto& entry : fs::directory_iterator(filepath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".fbx") {
				std::string filename = entry.path().filename().string();
				size_t dotPos = filename.find('.');
				if (dotPos != std::string::npos) {
					uniqueNames.insert(filename.substr(0, dotPos)); // Extract base name
				}
			}
		}

		// Output unique base names
		for (const auto& name : uniqueNames) {
			std::string fileP = filepath + "/" + name + ".fbx";

			std::cout << "Processing: " << fileP << '\n';
			AnimationAdd(name, fileP);

		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error accessing directory: " << e.what() << std::endl;
	}



}


void GraphicsSystem::AnimationAdd(const std::string& name, const std::string& path)
{

	std::unique_ptr<Model> animModel = std::make_unique<Model>(path, false);
	std::unique_ptr<Animation> corgiWalk = std::make_unique<Animation>(path, animModel.get());
	std::unique_ptr<Animator> animTor = std::make_unique<Animator>(corgiWalk.get());

	g_ResourceManager.ModelMap[name] = std::move(animModel);
	g_ResourceManager.AnimationMap[name] = std::move(corgiWalk);
	g_ResourceManager.AnimatorMap[name] = std::move(animTor);


	// Allocate model dynamically

	g_ResourceManager.addModelNames(name);



	std::cout << "Animation and Animator: " << name << " added successfully!\n";
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
	//g_AssetManager.GetShader("Shader3D").SetUniform("viewPos", camera.Position);


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
				//			g_ResourceManager.getModel(graphicsComp.getModelName())->texture_cnt = 0;
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


void GraphicsSystem::AddDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
{
	debugLines.push_back({ start, end, color });
}

void GraphicsSystem::RenderDebugLines()
{
	glLineWidth(2.0f);

	if (debugLines.empty())
		return; // Nothing to draw

	// Step 1: If we haven�t created a VAO/VBO for debug lines yet, create them once:
	if (debugLineVAO == 0)
	{
		glGenVertexArrays(1, &debugLineVAO);
		glBindVertexArray(debugLineVAO);

		glGenBuffers(1, &debugLineVBO);
		glBindBuffer(GL_ARRAY_BUFFER, debugLineVBO);

		// Each line has 2 endpoints, each endpoint has 6 floats: (pos.x, pos.y, pos.z, color.r, color.g, color.b)
		// We�ll enable 2 attributes: location 0 for position, location 1 for color
		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		// Color attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	// Step 2: Build the CPU array of line data
	// For N lines, we have 2*N vertices (start + end)
	std::vector<float> lineData;
	lineData.reserve(debugLines.size() * 12); // 2 points * 6 floats
	for (auto& line : debugLines)
	{
		// Start point
		lineData.push_back(line.start.x);
		lineData.push_back(line.start.y);
		lineData.push_back(line.start.z);
		lineData.push_back(line.color.r);
		lineData.push_back(line.color.g);
		lineData.push_back(line.color.b);

		// End point
		lineData.push_back(line.end.x);
		lineData.push_back(line.end.y);
		lineData.push_back(line.end.z);
		lineData.push_back(line.color.r);
		lineData.push_back(line.color.g);
		lineData.push_back(line.color.b);
	}

	// Step 3: Upload data to the GPU
	glBindVertexArray(debugLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, debugLineVBO);
	glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(float), lineData.data(), GL_DYNAMIC_DRAW);

	// Step 4: Use a basic line shader (or reuse "Shader3D" if it can do untextured lines)
	// Suppose we have a "DebugLineShader" loaded in g_AssetManager:
	OpenGLShader& debugShader = g_AssetManager.GetShader("DebugLineShader");
	debugShader.Use();

	// We need a camera's view/projection. If you want to use the active camera, do:
	debugShader.SetUniform("view", camera_render.GetViewMatrix());
	debugShader.SetUniform("gammaValue", gammaValue);
	debugShader.SetUniform("projection",
		glm::perspective(glm::radians(45.0f), (float)g_WindowX / (float)g_WindowY, 0.1f, 100.0f));

	// Step 5: Issue the draw call
	glDrawArrays(GL_LINES, 0, (GLsizei)(debugLines.size() * 2));

	debugShader.UnUse();

	// Step 6: Clear the debug lines for the next frame
	debugLines.clear();
}

void GraphicsSystem::RenderScence(OpenGLShader& shader)
{
	auto allEntities = g_Coordinator.GetAliveEntitiesSet();

	glm::mat4 View_ = camera_render.GetViewMatrix();
	glm::mat4 Projection_ = glm::perspective(glm::radians(45.0f), (g_WindowY > 0) ? ((float)g_WindowX / (float)g_WindowY) : 1, 0.1f, 100.0f);

	for (auto& entity : allEntities) {
		if (!g_Coordinator.HaveComponent<TransformComponent>(entity) ||
			!g_Coordinator.HaveComponent<GraphicsComponent>(entity))
		{
			continue;
		}

		auto& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
		//if no model , skip

		if (graphicsComp.getModel() == nullptr)
		{
			continue;
		}

		if (graphicsComp.getModelName() == "Wall" || graphicsComp.getModelName() == "Ceiling")
		{
			continue;
		}

		// Optionally check that this entity should cast a shadow.
		auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
		glm::mat4 worldMatrix = transformComp.GetWorldMatrix();

		// Set the model matrix for the shadow shader:
		shader.SetUniform("vertexTransform", worldMatrix);
		// Render the entity’s model (make sure your model class has a method for shadow rendering)
		graphicsComp.getModel()->DrawForPicking();
	}

}

void GraphicsSystem::RenderLightPos()
{
	auto allEntities = g_Coordinator.GetAliveEntitiesSet();

	glm::mat4 View_ = camera_render.GetViewMatrix();
	glm::mat4 Projection_ = glm::perspective(glm::radians(45.0f), (g_WindowY > 0) ? ((float)g_WindowX / (float)g_WindowY) : 1, 0.1f, 100.0f);

	for (auto& entity : allEntities) {

		if (!g_Coordinator.HaveComponent<TransformComponent>(entity) || !g_Coordinator.HaveComponent<LightComponent>(entity))
		{
			continue;
		}
		auto& transformComp = g_Coordinator.GetComponent<TransformComponent>(entity);
		auto& lightComp = g_Coordinator.GetComponent<LightComponent>(entity);

		std::shared_ptr<TransformSystem> transformSystem = g_Coordinator.GetSystem<TransformSystem>();

		transformComp.SetScale(glm::vec3(0.1f) * lightComp.getIntensity());

		g_AssetManager.GetShader("Wireframe").Use();
		glm::mat4 worldMatrix = transformComp.GetWorldMatrix();
		g_AssetManager.GetShader("Wireframe").SetUniform("vertexTransform", worldMatrix);
		g_AssetManager.GetShader("Wireframe").SetUniform("view", View_);
		g_AssetManager.GetShader("Wireframe").SetUniform("projection", Projection_);
		g_AssetManager.GetShader("Wireframe").SetUniform("objectColor", lightComp.getColor());
		g_ResourceManager.getModel("sphere")->DrawWireFrame();
		g_AssetManager.GetShader("Wireframe").UnUse();

		AddDebugLine(transformComp.GetPosition(), transformComp.GetPosition() + lightComp.getDirection(), glm::vec3(1.0f, 1.0f, 1.0f));
	}

}





