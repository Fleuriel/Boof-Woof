#pragma once

#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"

Entity playerEnt{};
CameraController* cameraController = nullptr;

class Level1 : public Level
{
	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/TestScene.json");
		g_BoneCatcher.OnLoad();

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
			g_BoneCatcher.OnInitialize();
		}
	}

	void UpdateLevel(double deltaTime)
	{
		cameraController->Update(static_cast<float>(deltaTime));
		g_BoneCatcher.OnUpdate(deltaTime);

		if (g_Input.GetKeyState(GLFW_KEY_TAB) >= 1)
		{
			cameraController->ToggleCameraMode();
		}

		if (g_Input.GetKeyState(GLFW_KEY_C) >= 1)
		{
			g_BoneCatcher.Stop();
		}

		// Space to go back mainmenu
		if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) >= 1)
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