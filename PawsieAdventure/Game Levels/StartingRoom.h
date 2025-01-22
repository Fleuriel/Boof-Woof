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


	void LoadLevel()
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/StartingRoom.json");
		//g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/BedRoomMusic.wav", true,  "BGM");

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

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail")
				{
					scentEntity = entity;
					break;
				}
			}
		}

		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "BedRoomMusic")
				{
					BedRoomBGM = entity;
					break;
				}
			}
		}
		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "CorgiBark1")
				{
					CorgiBark = entity;
					break;
				}
			}
		}
		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "CorgiSniff")
				{
					CorgiSniff = entity;
					break;
				}
			}
		}



		auto& music = g_Coordinator.GetComponent<AudioComponent>(BedRoomBGM);
		music.SetAudioSystem(&g_Audio);
		music.PlayAudio();


		auto& music1 = g_Coordinator.GetComponent<AudioComponent>(CorgiBark);
		music1.SetAudioSystem(&g_Audio);


		auto& music2 = g_Coordinator.GetComponent<AudioComponent>(CorgiSniff);
		music2.SetAudioSystem(&g_Audio);



		g_Window->HideMouseCursor();
	}

	void InitLevel()
	{
		// Ensure player entity is valid
		cameraController = new CameraController(playerEnt);
		camerachange = false;
	}

	bool teb_last = false;

	void UpdateLevel(double deltaTime)
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
			//g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/CorgiBark1.wav", false, "SFX");
			if (g_Coordinator.HaveComponent<AudioComponent>(CorgiBark)) {
				auto& music1 = g_Coordinator.GetComponent<AudioComponent>(CorgiBark);
				music1.PlayAudio();
			}
			bark = true;
		}

		if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 0)
		{
			bark = false;
		}

		if (g_Input.GetKeyState(GLFW_KEY_R) >= 1 && !sniff)
		{
			//g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/CorgiSniff.wav", false, "SFX");
			if (g_Coordinator.HaveComponent<AudioComponent>(CorgiSniff)) {
				auto& music2 = g_Coordinator.GetComponent<AudioComponent>(CorgiSniff);
				music2.PlayAudio();
			}
			opacity.setParticleColor(glm::vec4(0.09019608050584793f, 0.7843137383460999f, 0.8549019694328308f, 1.0f));
			sniff = true;
		}

		if (g_Input.GetKeyState(GLFW_KEY_R) == 0)
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

		//// Space to go back mainmenu
		//if (g_Input.GetKeyState(GLFW_KEY_ESCAPE) >= 1)
		//{
		//	g_LevelManager.SetNextLevel("MainMenu");
		//	g_Window->ShowMouseCursor();
		//}
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
		//g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/BedRoomMusic.wav");
		if (g_Coordinator.HaveComponent<AudioComponent>(BedRoomBGM)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(BedRoomBGM);
			music.StopAudio();
		}
		g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
	}

private:
	bool camerachange = false;
};