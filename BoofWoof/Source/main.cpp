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
#include "Coordinator.h"
#include <TestComponent.h>
#include <TestSystem.h>



#include <WindowComponent.h>
#include <WindowSystem.h>
#include <GraphicsSystem.h>
#include <GraphicsComponent.h>

// Global Variables
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

	std::cout << "here1\n";

	// Initialize the coordinator
	gCoordinator.Init();

	// Register components with the coordinator
	gCoordinator.RegisterComponent<TestComponent>();
	gCoordinator.RegisterComponent<GraphicsComponent>();
	gCoordinator.RegisterComponent<WindowComponent>();

	// Register systems with the coordinator
	auto testSystem = gCoordinator.RegisterSystem<TestSystem>();
	auto graphicsSystem = gCoordinator.RegisterSystem<GraphicsSystem>();
	auto windowSystem = gCoordinator.RegisterSystem<WindowSystem>();

	Signature signature;
	signature.set(gCoordinator.GetComponentType<TestComponent>());
	gCoordinator.SetSystemSignature<TestSystem>(signature);

	Signature windowSystemSignature;
	windowSystemSignature.set(gCoordinator.GetComponentType<WindowComponent>()); // Add WindowComponent bit to signature
	gCoordinator.SetSystemSignature<WindowSystem>(windowSystemSignature);

	Signature graphicsSystemSignature;
	graphicsSystemSignature.set(gCoordinator.GetComponentType<GraphicsComponent>()); // Add WindowComponent bit to signature
	gCoordinator.SetSystemSignature<GraphicsSystem>(graphicsSystemSignature);

	Entity windowEntity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(windowEntity, WindowComponent{});// Add a WindowComponent to the entity

	Entity graphicsEntity = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(graphicsEntity, GraphicsComponent{ /* initialization data */ });


	// INIT THE WINDOW AND PIPELINE.
	WindowComponent& windowComp = gCoordinator.GetComponent<WindowComponent>(windowEntity);

	// Initialize the window with the window component
	windowSystem->initWindow(windowComp);


	GraphicsComponent& graphicsComp = gCoordinator.GetComponent<GraphicsComponent>(graphicsEntity);
	// Call the initialization function with the component
	graphicsSystem->initGraphicsPipeline(graphicsComp);

	std::vector<Entity> entities(MAX_ENTITIES);

	std::cout << "here2\n";

	GraphicsSystem graphicSystem;

	std::cout << "here331\n";

	//for(auto& entity : entities)
	//{
	//	entity = gCoordinator.CreateEntity();
	//	gCoordinator.AddComponent<TestComponent>(entity, TestComponent{ 1, 2 });
	//	//gCoordinator.AddComponent<GraphicsComponent>(entity);
	//}
	// Now that the window system is ready and we have an entity with WindowComponent, we can initialize the window
	AssetManager assetManager;

	
	if (!windowSystem) {
		std::cerr << "WindowSystem is null!" << std::endl;
		return 1;
	}

	// Check if Loading of shaders have error
	bool test = assetManager.LoadShaders();
	if (!test)
	{
		std::cout << "Failed to Load Shaders\n";
		return 0;
	}

	// Initialize Previous Time with NOW()
	previousTime = std::chrono::high_resolution_clock::now();
	
	// Initialize ImGui
	//GraphicsUserInterface::Initialize();

	// 
	float dt = 0.0f;
	// While Loop
	while (!glfwWindowShouldClose(windowComp.window))
	{
		if (currentState == STATE::END)
		{
			std::cout << " Current State is END\n";
			break;
		}

		auto startTime = std::chrono::high_resolution_clock::now();

		testSystem->Update(dt);

		auto stopTime = std::chrono::high_resolution_clock::now();

		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();

		// Render GUI
		//GraphicsUserInterface::RenderGUI();

		
		graphicSystem.UpdateLoop(graphicsComp);

		previousState = currentState;
		currentState = nextState;
		// Update Graphics
		//graphicObject.UpdateLoop();

		// Update the ImGui
		//GraphicsUserInterface::Update();
		
		// Swap Buffers and Poll the events
		glfwSwapBuffers(windowComp.window);
		glfwPollEvents();
		// After everything happened, set the current time.
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousTime);
		previousTime = currentTime;
	}

	// Cleanup
	//GraphicsUserInterface::Cleanup();


	// Terminate
	glfwTerminate();
	std::cout << " End of Program" << std::endl;
	return 0;
}