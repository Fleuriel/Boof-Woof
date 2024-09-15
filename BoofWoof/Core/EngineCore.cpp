#include "EngineCore.h"

std::shared_ptr<TestSystem> mTestSystem;

void EngineCore::OnInit()
{
	// initialize window stuff, set window height n width n wtv

	// register components here
	g_Coordinator.Init();
	g_Coordinator.RegisterComponent<TestComponent>();


	// setting global pointer
	gCore = this;

	// register system & signatures
	mTestSystem = g_Coordinator.RegisterSystem<TestSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<TestComponent>());
		g_Coordinator.SetSystemSignature<TestSystem>(signature);
	}
}

void EngineCore::OnUpdate()
{
	// window update
	
	// input update

	// system updates
	mTestSystem->Update(0.0f);

	// ur glfw swapp buffer thingy
}

void EngineCore::OnShutdown()
{
	// shutdown all systems & delete window
}
