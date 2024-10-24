#include "EngineCore.h"

std::shared_ptr<GraphicsSystem> mGraphicsSys;
std::shared_ptr<AudioSystem> mAudioSys;
std::shared_ptr<LogicSystem> mLogicSys;
std::shared_ptr<FontSystem> mFontSys;

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
	g_Coordinator.RegisterComponent<AudioComponent>();

	g_Coordinator.RegisterComponent<BehaviourComponent>();

	// setting global pointer
	g_Core = this;

	// Set up your global managers


	// register system & signatures

	mLogicSys = g_Coordinator.RegisterSystem<LogicSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<BehaviourComponent>());
		g_Coordinator.SetSystemSignature<LogicSystem>(signature);
	}


	mGraphicsSys = g_Coordinator.RegisterSystem<GraphicsSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<TransformComponent>());
		signature.set(g_Coordinator.GetComponentType<GraphicsComponent>());
		g_Coordinator.SetSystemSignature<GraphicsSystem>(signature);
	}

	mAudioSys = g_Coordinator.RegisterSystem<AudioSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<AudioComponent>());
		g_Coordinator.SetSystemSignature<AudioSystem>(signature);
	}

	mFontSys = g_Coordinator.RegisterSystem<FontSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<TransformComponent>());
		g_Coordinator.SetSystemSignature<FontSystem>(signature);
	}


	// init system
	mLogicSys->Init();
	mGraphicsSys->initGraphicsPipeline();
	//mFontSys->init();
		

	// Just leave this part at the most bottom
	m_AccumulatedTime = 0.0;		// elapsed time
	m_CurrNumSteps = 0;
}

void EngineCore::OnUpdate()
{
	m_CurrNumSteps = 0;
	m_DeltaTime = m_EndTime - m_StartTime;	// start at 0
	m_StartTime = g_Timer.GetCurrentTime();
	m_AccumulatedTime += m_DeltaTime;

	while (m_AccumulatedTime >= m_FixedDT)
	{
		m_AccumulatedTime -= m_FixedDT;
		m_CurrNumSteps++;
	}

	// Cap at 4 to avoid performance hit due to too many steps in single frame
	if (m_CurrNumSteps >= 4)
	{
		m_CurrNumSteps = 4;
	}

	// window update
	g_Window->OnUpdate();

	//sound test
	/*
	if (g_Input.GetKeyState(GLFW_KEY_P) == 1 && g_Coordinator.HaveComponent<AudioComponent>(g_Coordinator.GetEntityId(0))) {

		std::cout << "P key pressed, playing explosion sound..." << std::endl;
		mAudioSys->Play(g_Coordinator.GetEntityId(0));  // Play the sound associated with the player entity
	}
	*/
	



	// input update
	g_Input.UpdateStatesForNextFrame();
	if (g_Input.GetKeyState(GLFW_KEY_H) == 1) {
		std::cout << "Press\n";
	}
	if (g_Input.GetKeyState(GLFW_KEY_H) == 2) {
		std::cout << "Hold\n";
	}



	//Transition
	g_SceneManager.Update((float)m_DeltaTime);

	// system updates
	{
		// Logic
		mLogicSys->Update();
		m_LogicDT = g_Timer.GetElapsedTime();
	}

	{
		// Graphics
		mGraphicsSys->UpdateLoop();
		m_GraphicsDT = g_Timer.GetElapsedTime();
		
	}
	{
		mAudioSys->Update();
	}



	// keep this at the end
	m_ElapsedDT = m_GraphicsDT + m_LogicDT; // to add more DT when more systems comes up
	m_EndTime = g_Timer.GetCurrentTime();

	// input update
	g_Input.UpdateStatesForNextFrame();
}

void EngineCore::OnShutdown()
{
	// Shutdown window and other systems
	mLogicSys->Shutdown();
	g_Window->OnShutdown();
}