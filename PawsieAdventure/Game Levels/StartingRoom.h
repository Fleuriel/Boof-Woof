#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"

class StartingRoom : public Level
{
public:
	Entity playerEnt{}, scentEntity{};
	CameraController* cameraController = nullptr;
	bool bark{ false }, sniff{ false };

	Entity BedRoomBGM{}, CorgiBark{}, CorgiSniff{};


	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/StartingRoom.json");
		
		g_ChangeText.OnInitialize();

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"Player", [&](Entity entity) { playerEnt = entity; }},
			{"ScentTrail", [&](Entity entity) { scentEntity = entity; }},
			{"BedRoomMusic", [&](Entity entity) { BedRoomBGM = entity; }},
			{"CorgiBark1", [&](Entity entity) { CorgiBark = entity; }},
			{"CorgiSniff", [&](Entity entity) { CorgiSniff = entity; }}
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


				if (g_Coordinator.HaveComponent<AudioComponent>(entity))
				{
					auto& music = g_Coordinator.GetComponent<AudioComponent>(entity);
					music.SetAudioSystem(&g_Audio);

					if (metadata.GetName() == "BedRoomMusic")
					{
						music.PlayAudio();
					}
				}

				// Exit early if all entities are found
				if (playerEnt && scentEntity && BedRoomBGM && CorgiBark && CorgiSniff)
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
	}

	bool teb_last = false;

	void UpdateLevel(double deltaTime) override
	{
		if (!g_IsPaused) 
		{
			if (!camerachange)
			{
				cameraController->ChangeToFirstPerson(g_Coordinator.GetComponent<CameraComponent>(playerEnt));
				camerachange = true;
			}
			cameraController->Update(static_cast<float>(deltaTime));

			auto& opacity = g_Coordinator.GetComponent<ParticleComponent>(scentEntity);

			if (!g_ChangeText.shutted)
			{
				g_ChangeText.OnUpdate(deltaTime);
			}
			else 
			{
				// let the change text finish first then allow pauseLogic
				pauseLogic::OnUpdate();
			}

			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);
			}

			if (g_Input.GetKeyState(GLFW_KEY_TAB) >= 1)
			{
				if (!teb_last)
				{
					teb_last = true;
					cameraController->ShakePlayer(1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
				}
			}
			else
			{
				teb_last = false;
			}

			//if (g_Input.GetKeyState(GLFW_KEY_O) >= 1) 
			//{
			//	cameraController->ShakeCamera(1.0f, glm::vec3(0.1f,0.1f,0.1f));
			//}

			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 1 && !bark)
			{
				if (g_Coordinator.HaveComponent<AudioComponent>(CorgiBark))
				{
					auto& music1 = g_Coordinator.GetComponent<AudioComponent>(CorgiBark);
					music1.PlayAudio();
				}
				bark = true;
			}

			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 0)
			{
				bark = false;
			}

			if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && !sniff)
			{
				if (g_Coordinator.HaveComponent<AudioComponent>(CorgiSniff))
				{
					auto& music2 = g_Coordinator.GetComponent<AudioComponent>(CorgiSniff);
					music2.PlayAudio();
				}

				opacity.setParticleColor(glm::vec4(0.09019608050584793f, 0.7843137383460999f, 0.8549019694328308f, 1.0f));
				sniff = true;
			}

			if (g_Input.GetKeyState(GLFW_KEY_E) == 0)
			{
				sniff = false;
			}

			if (g_Checklist.shutted)
			{
				if (g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetLastCollidedObjectName() == "WallHole")
				{
					g_LevelManager.SetNextLevel("TimeRush");
				}
			}
		}	

		if (g_ChangeText.shutted) 
		{
			pauseLogic::OnUpdate();
		}
	}

	void FreeLevel() override
	{
		if (cameraController)
		{
			delete cameraController;
			cameraController = nullptr;
		}
	}

	void UnloadLevel() override
	{
		//g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/BedRoomMusic.wav");
		//if (g_Coordinator.HaveComponent<AudioComponent>(BedRoomBGM)) {
		//	auto& music = g_Coordinator.GetComponent<AudioComponent>(BedRoomBGM);
		//	music.StopAudio();
		//}

		g_Audio.Stop(BedRoomBGM);

		//g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
	}

private:
	bool camerachange = false;
};