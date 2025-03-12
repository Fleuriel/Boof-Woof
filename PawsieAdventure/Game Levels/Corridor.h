#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../Systems/CameraController/CameraController.h"
#include "../GSM/GameStateMachine.h"

class Corridor : public Level
{
	Entity playerEnt{};
	CameraController* cameraController = nullptr;
	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/SecondCorridor.json");
		//g_TimerTR.OnInitialize();

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"Player", [&](Entity entity) { playerEnt = entity; }},
		};


		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
				auto it = nameToAction.find(metadata.GetName());

				if (it != nameToAction.end())
				{
					it->second(entity);
				}

				// Exit early if all entities are found
				if (playerEnt)
				{
					break;
				}
			}
		}
		g_Window->HideMouseCursor();
	}

	void InitLevel() override
	{
		// Ensure player entity is valid
		cameraController = new CameraController(playerEnt);
		camerachange = false;

		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::OUTOFLIBRARY);

		g_UI.OnInitialize();
	}

	void UpdateLevel(double deltaTime) override
	{
		if (!g_DialogueText.dialogueActive)
		{
			pauseLogic::OnUpdate();
		}

		if (!g_IsPaused)
		{
			if (!camerachange)
			{
				cameraController->ChangeToFirstPerson(g_Coordinator.GetComponent<CameraComponent>(playerEnt));
				camerachange = true;
			}

			cameraController->Update(static_cast<float>(deltaTime));

			g_UI.OnUpdate(static_cast<float>(deltaTime));
			g_DialogueText.OnUpdate(deltaTime);

			if (g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetLastCollidedObjectName() == "CastleWallDoor")
			{
				auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
				if (loading)
				{
					// Pass in the name of the real scene we want AFTER the loading screen
					loading->m_NextScene = "MainHall";
					g_LevelManager.SetNextLevel("LoadingLevel");
				}
			}
			
		}
	}

	void FreeLevel() override
	{
		if (cameraController)
		{
			delete cameraController;
			cameraController = nullptr;
		}

		g_UI.OnShutdown();
		g_DialogueText.OnShutdown();
	}

	void UnloadLevel() override
	{
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
	}

private:
	bool camerachange = false;
};
	