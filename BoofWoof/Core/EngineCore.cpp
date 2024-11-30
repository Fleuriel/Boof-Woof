#include "pch.h"
#include "EngineCore.h"
#include "../Core/Reflection/ReflectionManager.hpp"

std::shared_ptr<GraphicsSystem> mGraphicsSys;
//std::shared_ptr<AnimationComponent> m_AnimationSys;
std::shared_ptr<AudioSystem> mAudioSys;
std::shared_ptr<LogicSystem> mLogicSys;
std::shared_ptr<FontSystem> mFontSys;
std::shared_ptr<MyPhysicsSystem> mPhysicSys;
std::shared_ptr<ParticleComponent> mParticleSys;
std::shared_ptr<TransformSystem> mTransformSys;


void EngineCore::OnInit()
{
	// if need to deserialize anything
	
	// initialize window stuff, set window height n width n wtv
	// tempo b4 serialization
	g_WindowX = 1920;
	g_WindowY = 1080;
	g_Window = new Window(g_WindowX, g_WindowY, "Pawsie Adventure");
	g_Window->SetWindowWidth(g_WindowX);
	g_Window->SetWindowHeight(g_WindowY);

	g_Window->OnInitialize();

	// register components here
	g_Coordinator.Init();
	g_Coordinator.RegisterComponent<MetadataComponent>();
	g_Coordinator.RegisterComponent<TransformComponent>();
	g_Coordinator.RegisterComponent<GraphicsComponent>();
	g_Coordinator.RegisterComponent<AnimationComponent>();
	g_Coordinator.RegisterComponent<MaterialComponent>();
	g_Coordinator.RegisterComponent<AudioComponent>();
	g_Coordinator.RegisterComponent<CollisionComponent>();
	g_Coordinator.RegisterComponent<BehaviourComponent>();
	g_Coordinator.RegisterComponent<CameraComponent>();
	g_Coordinator.RegisterComponent<ParticleComponent>();
	g_Coordinator.RegisterComponent<HierarchyComponent>();
	g_Coordinator.RegisterComponent<LightComponent>();

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
		signature.set(g_Coordinator.GetComponentType<GraphicsComponent>());
		signature.set(g_Coordinator.GetComponentType<TransformComponent>());
		signature.set(g_Coordinator.GetComponentType<MaterialComponent>());
		signature.set(g_Coordinator.GetComponentType<AnimationComponent>());
		signature.set(g_Coordinator.GetComponentType<CameraComponent>());
		signature.set(g_Coordinator.GetComponentType<ParticleComponent>());
		signature.set(g_Coordinator.GetComponentType<LightComponent>());
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

	mPhysicSys = g_Coordinator.RegisterSystem<MyPhysicsSystem>();
	{
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<TransformComponent>());
		g_Coordinator.SetSystemSignature<MyPhysicsSystem>(signature);
	}


	mTransformSys = g_Coordinator.RegisterSystem<TransformSystem>(); {
		Signature signature;
		signature.set(g_Coordinator.GetComponentType<TransformComponent>());
		g_Coordinator.SetSystemSignature<TransformSystem>(signature);
	}


	// init system
	mLogicSys->Init();
	mGraphicsSys->initGraphicsPipeline();
	mPhysicSys->InitializeJolt();
	//mFontSys->init();
		

	// Just leave this part at the most bottom
	m_AccumulatedTime = 0.0;		// elapsed time
	m_CurrNumSteps = 0;

	ReflectionManager::Instance().RegisterComponentType<MetadataComponent>("MetadataComponent");
	ReflectionManager::Instance().RegisterComponentType<TransformComponent>("TransformComponent");
	ReflectionManager::Instance().RegisterComponentType<GraphicsComponent>("GraphicsComponent");
	ReflectionManager::Instance().RegisterComponentType<AnimationComponent>("AnimationComponent");
	ReflectionManager::Instance().RegisterComponentType<AudioComponent>("AudioComponent");
	ReflectionManager::Instance().RegisterComponentType<BehaviourComponent>("BehaviourComponent");
	ReflectionManager::Instance().RegisterComponentType<CollisionComponent>("CollisionComponent");
	ReflectionManager::Instance().RegisterComponentType<CameraComponent>("CameraComponent");
	ReflectionManager::Instance().RegisterComponentType<ParticleComponent>("ParticleComponent");
	ReflectionManager::Instance().RegisterComponentType<MaterialComponent>("MaterialComponent");
	ReflectionManager::Instance().RegisterComponentType<HierarchyComponent>("HierarchyComponent");
	ReflectionManager::Instance().RegisterComponentType<LightComponent>("LightComponent");
}

void EngineCore::OnUpdate()
{
	m_CurrNumSteps = 0;
	m_DeltaTime = m_EndTime - m_StartTime;	// start at 0
	m_StartTime = GetCurrentTime();
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


	// input update
	g_Input.UpdateStatesForNextFrame();

	//Transition
	g_SceneManager.Update((float)m_DeltaTime);

	// system updates
	{
		// Logic
		Timer gamelogicTimer;
		mLogicSys->Update();
		m_LogicDT = gamelogicTimer.GetElapsedTime();
	}

	{
		// Physics
		Timer physicsTimer;
		mPhysicSys->OnUpdate(static_cast<float>(m_DeltaTime));  // Update physics
		m_PhysicsDT = physicsTimer.GetElapsedTime();
	}

	{
		mAudioSys->Update();
	}

	{
		mTransformSys->Update();
	}


	{
		// Graphics
		Timer graphicsTimer;
		mGraphicsSys->UpdateLoop();
		m_GraphicsDT = graphicsTimer.GetElapsedTime();
	}



	// keep this at the end
	m_ElapsedDT = m_LogicDT + m_PhysicsDT + m_GraphicsDT; // to add more DT when more systems comes up
	m_EndTime = GetCurrentTime();

	// input update
	g_Input.UpdateStatesForNextFrame();
}

void EngineCore::OnShutdown()
{
	// Shutdown window and other systems
	mLogicSys->Shutdown();
	mPhysicSys->Cleanup();
	g_Window->OnShutdown();
}