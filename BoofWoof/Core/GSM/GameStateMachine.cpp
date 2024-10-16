/**************************************************************************
 * @file GameStateMachine.cpp
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file allows changing of states, while having the added functionality
 * of getting FPS and DeltaTime in the game.
 *
 *
 *************************************************************************/

#include "GameStateMachine.h"
#include "EngineCore.h"

GameStates g_CurrentState{}, g_PreviousState{}, g_NextState{};

void UpdateGSM()
{
	while (g_CurrentState != GameStates::QUIT)
	{
		if (g_CurrentState != GameStates::RESTART)
		{
			g_LevelManager.LoadLevel();
		}
		else
		{
			// When restart level is triggered
			g_LevelManager.SetNextLevel(g_LevelManager.GetPreviousLevel());
			g_LevelManager.SetCurrentLevel(g_LevelManager.GetPreviousLevel());
		}

		g_LevelManager.InitLevel();

		// Game loop
		while (g_LevelManager.GetNextLevel() == g_LevelManager.GetCurrentLevel())
		{
			g_Core->OnUpdate();	// outside of the above loop to render once per frame for efficiency

			g_LevelManager.UpdateLevel(g_Core->m_DeltaTime);

			if (glfwWindowShouldClose(g_Window->GetGLFWWindow()) || g_LevelManager.GetNextLevel() == nullptr)
			{
				g_CurrentState = GameStates::QUIT;
				break;
			}
		}

		g_LevelManager.FreeLevel();

		if (g_NextState != GameStates::RESTART)
		{
			g_LevelManager.UnloadLevel();
		}

		g_LevelManager.SetPreviousLevel(g_LevelManager.GetCurrentLevel());
		g_LevelManager.SetCurrentLevel(g_LevelManager.GetNextLevel());
	}
}

//std::chrono::high_resolution_clock::time_point currentTime;
//std::chrono::high_resolution_clock::time_point previousTime;
//std::chrono::duration<double> deltaTime;
//
//double seconds;
//
//
//STATE currentState = STATE::START;
//STATE nextState = STATE::RUNNING;
//STATE previousState = STATE::STARTUPPREVIOUS;
//
///**************************************************************************
//* @brief On Startup Run:
//* @param NONE
//* @return VOID
//*************************************************************************/
//void StartUp()
//{
//	// Set States
//	currentState = STATE::START;
//	previousState = STATE::STARTUPPREVIOUS;
//	nextState = STATE::RUNNING;
//
//	// Change of States
//
//	currentState = nextState;
//
//}
//
//
///**************************************************************************
//* @brief  Changing States
//* @param  CURRENT STATE
//* @param  NEXT STATE
//* @return void
//*************************************************************************/
//void ChangeState(STATE Current, STATE Upcoming)
//{
//	//Swap States.
//	currentState = Upcoming;
//	previousState = Current;	
//}
//
//
//
//double FPS()
//{
//	return 1 / deltaTime.count();
//}
//
//double DeltaTime()
//{
//	return deltaTime.count();
//}
//
//
//bool IsTimeElapsed(unsigned int time) {
//	// only run once.
//	// Get Time Elapsed.
//	seconds += DeltaTime();
//#ifdef _DEBUG	
//	//std::cout << seconds << GetDT() << '\n';
//#endif
//	// refresh seconds to 0
//	if (seconds >= time) {
//		seconds = 0;
//		return true;
//
//	}
//	else
//		return false;
//}