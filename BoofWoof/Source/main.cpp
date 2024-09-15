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
#include "../Core/Graphics/Graphics.h"

#include "../Core/GSM/GameStateMachine.h"
#include "../Core/Graphics/GraphicsUserInterface.h"
#include "../Core/AssetManager/AssetManager.h"

#include "../Headers/TestSystem.h"
#include "EngineCore.h"

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

	// Check glfwInit
	if (!glfwInit())
	{
		return -1;
	}

	// Initialize Window
	Graphics::initWindow();

	//auto testSystem = g_Coordinator.RegisterSystem<TestSystem>();
	//Signature signature;
	//signature.set(g_Coordinator.GetComponentType<TestComponent>());
	//g_Coordinator.SetSystemSignature<TestSystem>(signature);

	Entity entity = g_Coordinator.CreateEntity();
	g_Coordinator.AddComponent<TestComponent>(entity, TestComponent{ .data = "Hello World" });

	//std::vector<Entity> entities(MAX_ENTITIES);

	//for (auto& entity : entities) {
	//	entity = g_Coordinator.CreateEntity();
	//	g_Coordinator.AddComponent<TestComponent>(entity, TestComponent{ .data = "Hello World" });
	//}


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

	// While Loop
	while (!glfwWindowShouldClose(newWindow))
	{
		if (currentState == STATE::END)
		{
			std::cout << " Current State is END\n";
			break;
		}

		gCore->OnUpdate();

		// Render GUI
		GraphicsUserInterface::RenderGUI();
		
		previousState = currentState;
		currentState = nextState;
		// Update Graphics - this part will crash ??
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