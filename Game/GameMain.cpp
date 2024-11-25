#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "../BoofWoof/Core/EngineCore.h"
#include "Level Manager/LevelManager.h"
#include "Game Levels/Splashscreen.h"
#include "Game Levels/MainMenu.h"
#include "Game Levels/Level1.h"
#include "GSM/GameStateMachine.h"
#include "Game Levels/Cutscene.h"

EngineCore* g_Core = nullptr;

void InitializeLevels() {
	// Register your levels here
	g_LevelManager.RegisterLevel("Splashscreen", new(Splashscreen));
	g_LevelManager.RegisterLevel("MainMenu", new(MainMenu));
	g_LevelManager.RegisterLevel("Cutscene", new(Cutscene));
	g_LevelManager.RegisterLevel("Level1", new(Level1));

	// Set the initial level
	g_LevelManager.Initialize("Level1");
	g_LevelManager.SetNextLevel("Level1");
	g_LevelManager.SetPreviousLevel("Level1");
}

int main()
{

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	//for use with debugging, change the value to the location of the mem leak per the crt debug info from the console
	// refer to stack frame and see where it all went wrong
	//_crtBreakAlloc = 372;

	g_Core = new EngineCore();
	g_Core->OnInit();

	InitializeLevels();
	UpdateGSM();

	// idk why if i comment out line 41, it crashes .-.
	while (!glfwWindowShouldClose(g_Window->GetGLFWWindow()))
	{
		//g_Core->OnUpdate();
	}

	g_Core->OnShutdown();

	delete g_Core;

	return 0;
}