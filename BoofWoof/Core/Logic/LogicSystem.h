#pragma once

#ifndef LOGIC_SYSTEM_H
#define LOGIC_SYSTEM_H

struct Behaviour;
class Script_to_Engine;

#include "../ECS/System.hpp"
#include "Behaviour.h"
#include "BehaviourInterface.h"
#include <map>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <wtypes.h>

class LogicSystem : public System
{
public:
	void Init();
	void Update();
	void Shutdown();

	void AddBehaviours(void* scriptBehavioursPtr);
	//void AddBehaviours(std::vector<std::unique_ptr<Behaviour_i>>* B_Vec);

private:
	// Keep track of all the behaviours
	 std::map<std::string, std::unique_ptr<Behaviour_i>> mBehaviours;
	 Script_to_Engine* mScriptEngine;
};

extern Entity g_Player;
const std::wstring DLL_MAIN_DIRECTORY = L"..\\ScriptWoof\\x64\\Debug\\ScriptWoof.dll";
const std::wstring DLL_COPY_DIRECTORY = L"..\\ScriptWoof.dll";
extern HINSTANCE hGetProcIDDLL;
#endif  // LOGIC_SYSTEM_H