#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"

Entity playerEnt{};
CameraController* cameraController = nullptr;

class Level1 : public Level
{
	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/TestScene.json");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Player")
				{
					playerEnt = entity;
					break;
				}
			}
		}
	}

	void InitLevel() 
	{
		if (playerEnt != Entity{}) 
		{ 
			// Ensure player entity is valid
			cameraController = new CameraController(playerEnt);
		}
	}

	void UpdateLevel(double deltaTime)
	{
		cameraController->Update(deltaTime);

		// Space to go back mainmenu
		if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1)
		{
			g_LevelManager.SetNextLevel("MainMenu");
		}
	}

	void FreeLevel() 
	{ 
		if (cameraController) 
		{
			delete cameraController;
			cameraController = nullptr;
		}
	}

	void UnloadLevel()
	{
		g_Coordinator.ResetEntities();
	}
};