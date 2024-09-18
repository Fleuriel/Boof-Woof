/**************************************************************************
 * @file Maino.cpp
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file is the root of the program, the main loop is here.
 *
 *
 *************************************************************************/

#include <iostream>

#include "../Core/GSM/GameStateMachine.h"
#include "../Core/AssetManager/AssetManager.h"

#include "EngineCore.h"

// Global Variables
EngineCore* gCore = nullptr;

/**************************************************************************
* @brief Main Function
* @return void
*************************************************************************/
int main()
{
	gCore = new EngineCore();
	gCore->OnInit();

	// Initializing States
	StartUp();

	std::cout << "Main here\n";

	// Initialize Previous Time with NOW()
	previousTime = std::chrono::high_resolution_clock::now();
	

	// While Loop
	while (!glfwWindowShouldClose(g_Window->GetGLFWWindow()))
	{
		if (currentState == STATE::END)
		{
			std::cout << " Current State is END\n";
			break;
		}

		gCore->OnUpdate();
		
		previousState = currentState;
		currentState = nextState;
		
		// Swap Buffers and Poll the events
		//glfwSwapBuffers(g_Window->GetGLFWWindow());
		//glfwPollEvents();

		// After everything happened, set the current time.
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousTime);
		previousTime = currentTime;
	}

	// Terminate
	glfwTerminate();
	std::cout << " End of Program" << std::endl;
	return 0;
}