#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"

class MainHall : public Level
{
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{};
	CameraController* cameraController = nullptr;

	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/CorgiVSRope.json");
		g_Audio.PlayBGM("../BoofWoof/Assets/Audio/BedRoomMusic.wav");

		g_ChangeText.OnInitialize();

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Player")
				{
					playerEnt = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope1")
				{
					RopeEnt = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Rope2")
				{
					RopeEnt2 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "DrawBridge")
				{
					BridgeEnt = entity;
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

			if (RopeEnt != Entity{})
			{
				g_RopeBreaker = RopeBreaker(playerEnt, RopeEnt, RopeEnt2, BridgeEnt);
			}
		}
	}

	bool teb_last = false;

	void UpdateLevel(double deltaTime)
	{
		cameraController->Update(static_cast<float>(deltaTime));

		if (!g_ChangeText.shutted)
		{
			g_ChangeText.OnUpdate(deltaTime);
		}

		if (!g_Checklist.shutted)
		{
			g_Checklist.OnUpdate(deltaTime);
		}

		g_RopeBreaker.OnUpdate(deltaTime);

		if (g_Input.GetKeyState(GLFW_KEY_TAB) >= 1)
		{
			if (!teb_last)
			{
				teb_last = true;
				cameraController->ToggleCameraMode();
			}
		}
		else
		{
			teb_last = false;
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
		g_Audio.StopBGM();
		g_Coordinator.ResetEntities();
	}
};