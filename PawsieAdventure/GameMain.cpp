#define _CRTDBG_MAP_ALLOC
#pragma warning(disable : 4005)
#include <stdlib.h>
#include <crtdbg.h>
#include "../BoofWoof/Core/EngineCore.h"
#include "Level Manager/LevelManager.h"
#include "GSM/GameStateMachine.h"
#include "Game Levels/Splashscreen.h"
#include "Game Levels/MainMenu.h"
#include "Game Levels/Cutscene.h"
#include "Game Levels/StartingRoom.h"
#include "Game Levels/Corridor.h"
#include "Game Levels/TimeRush.h"
#include "Game Levels/MainHall.h"
#include "Game Levels/LoadingLevel.h"
#include "Game Levels/CutsceneEnd.h"
#include "Game Levels/TYVM.h"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"

EngineCore* g_Core = nullptr;

void InitializeLevels()
{
    // Register your levels here
    g_LevelManager.RegisterLevel("Splashscreen", new Splashscreen());
    g_LevelManager.RegisterLevel("MainMenu", new MainMenu());
    g_LevelManager.RegisterLevel("Cutscene", new Cutscene());
    g_LevelManager.RegisterLevel("StartingRoom", new StartingRoom());
	g_LevelManager.RegisterLevel("Corridor", new Corridor());
    g_LevelManager.RegisterLevel("TimeRush", new TimeRush());
    g_LevelManager.RegisterLevel("MainHall", new MainHall());
    g_LevelManager.RegisterLevel("LoadingLevel", new LoadingLevel());
    g_LevelManager.RegisterLevel("CutsceneEnd", new CutsceneEnd());
	g_LevelManager.RegisterLevel("TYVM", new TYVM());

    // Set the initial level
    g_LevelManager.Initialize("Splashscreen");
    g_LevelManager.SetNextLevel("Splashscreen");
    g_LevelManager.SetPreviousLevel("Splashscreen");
}

int RunGame()
{
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

    // Uncomment and set the allocation number if debugging memory leaks
    //_crtBreakAlloc = 372;
    FilePaths::InitPaths();

    g_Core = new EngineCore();
    g_Core->OnInit();
    g_Coordinator.GetSystem<GraphicsSystem>()->lightOn = true;

#ifndef _DEBUG
    // Only toggle full screen in release mode
    g_Window->toggleFullScreen();
#endif

    InitializeLevels();
    UpdateGSM();

    // Main game loop
    while (!glfwWindowShouldClose(g_Window->GetGLFWWindow()))
    {
        // Uncomment if the core update logic is required
        // g_Core->OnUpdate();
    }

    g_Core->OnShutdown();

    delete g_Core;

    return 0;
}



//#ifdef _DEBUG
// Use `main` in debug mode
int main()
{
    return RunGame();
}
//#else
//#include <windows.h>
////// Use `WinMain` in release mode
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//    (void)hInstance;
//    (void)hPrevInstance;
//    (void)lpCmdLine;
//    (void)nCmdShow;
//    return RunGame();
//}
//#endif
