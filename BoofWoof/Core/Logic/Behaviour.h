#pragma once
#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H


#include "../ECS/pch.hpp"
#include "../ECS/Coordinator.hpp"
#include "LogicSystem.h"



// This struct should be in a common header between the engine and the scripts
// Common.h
struct Behaviour_i
{
	virtual ~Behaviour_i() = default;
	virtual void Init(Entity entity) = 0;
	virtual void Update(Entity entity) = 0;
	virtual void Destroy(Entity entity) = 0;
	virtual const char* getBehaviourName() = 0;	
};

struct engine_interface
{
	virtual ~engine_interface() = default;
	virtual void AddComponent(Entity entity, ComponentType type) = 0;
};


using GetScripts_cpp_t = std::vector<std::unique_ptr<Behaviour_i>>* (*)(engine_interface* pEI);

// This class should only be visible to the engine
#include  "Common.h"
class Engine : public engine_interface
{
	virtual void AddComponent(Entity entity, ComponentType type) {}
};


// Some CPP file in the engine.....

//auto pGetScripts = (GetScripts_cpp_t)GetProcAddress(hGetProcIDDLL, "GetScripts");
//if (!pGetScripts) pGetScripts ( ... );



// This class should only be visible to the DLL (scripts)
#include  "Common.h"
struct Behaviour : public Behaviour_i
{
	Behaviour(engine_interface& Engine) : m_Engine(Engine) {}
	virtual void Init(Entity entity) override {}
	virtual void Update(Entity entity) override {}
	virtual void Destroy(Entity entity) override {}
	engine_interface&	m_Engine;
};


//// New Script (DLL)


class myBehavior final : public Behaviour
{
	using Behaviour::Behaviour;

	virtual void Update(Entity entity) override
	{

	}

	virtual  const char* getBehaviourName() override
	{
		return "myBehavior";
	}
};



////// In some CPP file in the DLL you will have this function

extern "C"
{
	__declspec(dllexport) void* GetScripts( void* pEI_tmp )
	{
		engine_interface&                          EP          = *reinterpret_cast<engine_interface*>(pEI_tmp);
		std::vector<std::unique_ptr<Behaviour_i>>* pBehaviours = new std::vector<std::unique_ptr<Behaviour_i>>();

		// Add all the scripts here
		pBehaviours->emplace_back(new myBehavior(EP));


		return pBehaviours;
	}
}




#endif  // BEHAVIOUR_H