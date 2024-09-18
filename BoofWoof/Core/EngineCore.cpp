#include "EngineCore.h"

std::shared_ptr<GraphicsSystem> mGraphicsSys;
std::shared_ptr<WindowSystem> mWindowSys;

void EngineCore::OnInit()
{
	// initialize window stuff, set window height n width n wtv

	// register components here
	g_Coordinator.Init();
	g_Coordinator.RegisterComponent<GraphicsComponent>();
	g_Coordinator.RegisterComponent<WindowComponent>();

	// setting global pointer
	gCore = this;

	// Set up your global managers
	g_AssetManager.LoadShaders();


	// register system & signatures
	mGraphicsSys = g_Coordinator.RegisterSystem<GraphicsSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<GraphicsComponent>());
		g_Coordinator.SetSystemSignature<GraphicsSystem>(signature);
	}

	mWindowSys = g_Coordinator.RegisterSystem<WindowSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<WindowComponent>());
		g_Coordinator.SetSystemSignature<WindowSystem>(signature);
	}

	// tempo creation of entity for the systems
	Entity graphicsEntity = g_Coordinator.CreateEntity();
	g_Coordinator.AddComponent<GraphicsComponent>(graphicsEntity, GraphicsComponent{ /* initialization data */ });

	Entity windowEntity = g_Coordinator.CreateEntity();
	g_Coordinator.AddComponent(windowEntity, WindowComponent{});// Add a WindowComponent to the entity

	// init system
	GraphicsComponent& graphicsComp = g_Coordinator.GetComponent<GraphicsComponent>(graphicsEntity);
	mGraphicsSys->initGraphicsPipeline(graphicsComp);

	WindowComponent& windowComp = g_Coordinator.GetComponent<WindowComponent>(windowEntity);
	mWindowSys->initWindow(windowComp);

	if (!mWindowSys) {
		std::cerr << "WindowSystem is null!" << std::endl;
	}

	std::cout << "EngineCore Initialized!" << std::endl;
	std::cout << "Total entities: " << g_Coordinator.GetTotalEntities() << std::endl;
}

void EngineCore::OnUpdate()
{
	// window update
	
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
}
