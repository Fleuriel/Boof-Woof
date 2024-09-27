#include "EngineCore.h"

std::shared_ptr<GraphicsSystem> mGraphicsSys;

void EngineCore::OnInit()
{
	// if need to deserialize anything
	
	// initialize window stuff, set window height n width n wtv
	// tempo b4 serialization
	g_WindowX = 1920;
	g_WindowY = 1080;
	g_Window = new Window(g_WindowX, g_WindowY, "Boof Woof");
	g_Window->SetWindowWidth(g_WindowX);
	g_Window->SetWindowHeight(g_WindowY);

	g_Window->OnInitialize();

	// register components here
	g_Coordinator.Init();
	g_Coordinator.RegisterComponent<MetadataComponent>();
	g_Coordinator.RegisterComponent<TransformComponent>();
	g_Coordinator.RegisterComponent<GraphicsComponent>();

	// setting global pointer
	g_Core = this;

	// Set up your global managers
	//g_AssetManager.LoadShaders();

	// register system & signatures
	mGraphicsSys = g_Coordinator.RegisterSystem<GraphicsSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<TransformComponent>());
		signature.set(g_Coordinator.GetComponentType<GraphicsComponent>());
		g_Coordinator.SetSystemSignature<GraphicsSystem>(signature);
	}

	// init system
	mGraphicsSys->initGraphicsPipeline();

	// tempo creation of entity for the systems
	Entity graphicsEntity = g_Coordinator.CreateEntity();
	//// add transform component
	g_Coordinator.AddComponent<TransformComponent>(graphicsEntity, TransformComponent(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), graphicsEntity));
	//// add graphics component
	g_Coordinator.AddComponent<GraphicsComponent>(graphicsEntity, GraphicsComponent(&g_AssetManager.Models[0], graphicsEntity));
	g_Coordinator.GetComponent<GraphicsComponent>(graphicsEntity).SetModelID(0);



	//Entity graphics2DEntity = g_Coordinator.CreateEntity();
	//
	//g_Coordinator.AddComponent<TransformComponent>(graphics2DEntity, TransformComponent(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), graphics2DEntity));
	//g_Coordinator.AddComponent<GraphicsComponent>(graphics2DEntity, GraphicsComponent(g_AssetManager.Model2D[0],graphics2DEntity));




	std::cout << "Total entities: " << g_Coordinator.GetTotalEntities() << std::endl;
}

void EngineCore::OnUpdate()
{
	// window update
	g_Window->OnUpdate();

	// system updates
	//auto allEntities = g_Coordinator.GetAliveEntitiesSet();
	//for (auto& entity : allEntities)
	//{
	//	if (g_Coordinator.HaveComponent<GraphicsComponent>(entity)) 
	//	{
	//		auto graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(entity);
	//		mGraphicsSys->UpdateLoop(graphicsComp);
	//	}
	//}

	/*
	Test serialization
	if (inputSystem.GetKeyState(GLFW_KEY_P) == 1) {  // Save engine state
		std::cout << "P key pressed, saving engine state..." << std::endl;
		Serialization::SaveEngineState("Saves/engine_state.json");
		std::cout << "Engine state saved to Saves/engine_state.json" << std::endl;
	}

	if (inputSystem.GetKeyState(GLFW_KEY_O) == 1) {  // Reset entities
		std::cout << "O key pressed, resetting entities..." << std::endl;
		g_Coordinator.ResetEntities();
		std::cout << "Entities reset!" << std::endl;
	}

	if (inputSystem.GetKeyState(GLFW_KEY_L) == 1) {  // Load engine state
		std::cout << "L key pressed, loading engine state..." << std::endl;
		Serialization::LoadEngineState("Saves/engine_state.json");
		std::cout << "Engine state loaded from Saves/engine_state.json" << std::endl;
	}
	*/


	mGraphicsSys->UpdateLoop();
	// input update
	inputSystem.UpdateStatesForNextFrame();

	// ur glfw swapp buffer thingy
}

void EngineCore::OnShutdown()
{
	// Shutdown window and other systems
	g_Window->OnShutdown();
}