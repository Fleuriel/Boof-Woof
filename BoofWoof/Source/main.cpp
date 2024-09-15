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
#include "Graphics.h"

#include "GameStateMachine.h"
#include "GraphicsUserInterface.h"
#include "AssetManager.h"

#include "pch.h"
#include <Coordinator.h>
#include <TestSystem.h>


Coordinator gCoordinator;
/**************************************************************************
* @brief Main Function
* @return void
*************************************************************************/
int main()
{

	// Initializing States
	StartUp();

	

	// Check glfwInit
	if (!glfwInit())
	{
		return -1;
	}

	// Initialize Window
	Graphics::initWindow();
	
	// Initialize Coordinator
	gCoordinator.Init();
	
	gCoordinator.RegisterComponent<TestComponent>();
	auto testSystem = gCoordinator.RegisterSystem<TestSystem>();
	
	Signature signature;
	signature.set(gCoordinator.GetComponentType<TestComponent>());
	gCoordinator.SetSystemSignature<TestSystem>(signature);

	std::vector<Entity> entities(MAX_ENTITIES);

	for (auto& entity : entities) {
		entity = gCoordinator.CreateEntity();
		gCoordinator.AddComponent<TestComponent>(entity, TestComponent{ .data = "Hello World" });
	}
	

	
	

	// Check if Loading of shaders have error
	bool test = assetManager.LoadAll();
	if (!test)
	{
		std::cout << "\nLoadAll Failed\n";
		return 0;
	}

	// Initialize Graphics Pipeline
	Graphics::initGraphicsPipeline();

	// Initialize Previous Time with NOW()
	previousTime = std::chrono::high_resolution_clock::now();
	
	// Initialize ImGui
	GraphicsUserInterface::Initialize();

	
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
	// While Loop
	while (!glfwWindowShouldClose(newWindow))
	{
		if (currentState == STATE::END)
		{
			std::cout << " Current State is END\n";
			break;
		}

		testSystem->Update(0.0f);

		// Render GUI
		GraphicsUserInterface::RenderGUI();


		
		previousState = currentState;
		currentState = nextState;
		// Update Graphics
		graphicObject.UpdateLoop();

		// Update the ImGui
		GraphicsUserInterface::Update();
		
		// Swap Buffers and Poll the events
		glfwSwapBuffers(newWindow);
		glfwPollEvents();

		// After everything happened, set the current time.
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousTime);
		previousTime = currentTime;
	}

	// Cleanup
	GraphicsUserInterface::Cleanup();


	// Terminate
	glfwTerminate();
	std::cout << " End of Program" << std::endl;
	return 0;
}