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
	g_Coordinator.RegisterComponent<GraphicsComponent>();

	// setting global pointer
	gCore = this;

	// Set up your global managers
	//g_AssetManager.LoadShaders();


	// register system & signatures
	mGraphicsSys = g_Coordinator.RegisterSystem<GraphicsSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<GraphicsComponent>());
		g_Coordinator.SetSystemSignature<GraphicsSystem>(signature);
	}

	// tempo creation of entity for the systems
	Entity graphicsEntity = g_Coordinator.CreateEntity();
	g_Coordinator.AddComponent<GraphicsComponent>(graphicsEntity, GraphicsComponent{ /* initialization data */ });

	// init system
	GraphicsComponent& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(graphicsEntity);
	mGraphicsSys->initGraphicsPipeline(graphicsComp);

	std::cout << "EngineCore Initialized!" << std::endl;
	std::cout << "Total entities: " << g_Coordinator.GetTotalEntities() << std::endl;
}

void EngineCore::OnUpdate()
{
	// window update
	g_Window->OnUpdate();
	// input update

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

	mGraphicsSys->UpdateLoop();

	// ur glfw swapp buffer thingy
}

void EngineCore::OnShutdown()
{
	// shutdown all systems & delete window

	g_Window->OnShutdown();
}
