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
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{}, scentEntity1{}, scentEntity2{};
	CameraController* cameraController = nullptr;

	void LoadLevel()
	{
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/MainHall.json");
		g_Audio.PlayBGM("../BoofWoof/Assets/Audio/BedRoomMusic.wav");

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
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail1")
				{
					scentEntity1 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail2")
				{
					scentEntity2 = entity;
					break;
				}
			}
		}

		g_Window->HideMouseCursor();
	}

	void InitLevel()
	{
		cameraController = new CameraController(playerEnt);
		g_RopeBreaker = RopeBreaker(playerEnt, RopeEnt, RopeEnt2, BridgeEnt);
	}

	bool teb_last = false;

	void UpdateLevel(double deltaTime)
	{
		cameraController->Update(static_cast<float>(deltaTime));

		auto& opacity1 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity1);
		auto& opacity2 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity2);

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

		if (g_Input.GetKeyState(GLFW_KEY_R) >= 1)
		{
			opacity1.setParticleColor(glm::vec4(0.0470588244497776f, 0.6627451181411743f, 0.95686274766922f, 1.0f));
			opacity2.setParticleColor(glm::vec4(0.7960784435272217f, 0.0470588244497776f, 0.95686274766922f, 1.0f));
		}

		// Space to go back mainmenu
		if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) >= 1)
		{
			g_LevelManager.SetNextLevel("MainMenu");
			g_Window->ShowMouseCursor();
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
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
	}
};