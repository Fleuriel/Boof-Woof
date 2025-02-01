#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "LoadingLevel.h"


class TimeRush : public Level
{
	double timer = 0.0;
	double interval = 1.0; // Time interval in seconds
	int currentTextureIndex = 53; // Start from "Group53"
	Entity timerTextEntity{}, playerEnt{}, scentEntity1{}, scentEntity2{}, scentEntity3{}, scentEntity4{}, scentEntity5{}, scentEntity6{};
	CameraController* cameraController = nullptr;

	Entity TimeRushBGM{}, AggroDog{}, CorgiSniff{};


	double colorChangeTimer = 0.0;
	double colorChangeDuration = 3.0; // Duration for which the color change lasts
	double cooldownTimer = 10.0;
	double cooldownDuration = 10.0; // Cooldown duration
	bool isColorChanged = false;

	bool finishTR{ false };
	double TRtimer = 0.0;
	double TRlimit = 2.0f;

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/TimeRushPuzzle.json");
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/Timer.json");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"Player", [&](Entity entity) { playerEnt = entity; }},
			{"ScentTrail1", [&](Entity entity) { scentEntity1 = entity; }},
			{"ScentTrail2", [&](Entity entity) { scentEntity2 = entity; }},
			{"ScentTrail3", [&](Entity entity) { scentEntity3 = entity; }},
			{"ScentTrail4", [&](Entity entity) { scentEntity4 = entity; }},
			{"ScentTrail5", [&](Entity entity) { scentEntity5 = entity; }},
			{"ScentTrail6", [&](Entity entity) { scentEntity6 = entity; }},
			{"Group", [&](Entity entity) { timerTextEntity = entity; }},
			{"TimeRushBGM", [&](Entity entity) { TimeRushBGM = entity; }},
			{"AggressiveDogBarking", [&](Entity entity) { AggroDog = entity; }},
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

					if (metadata.GetName() == "TimeRushBGM" || metadata.GetName() == "AggressiveDogBarking")
					{
						music.PlayAudio();
					}
				}

				// Exit early if all entities are found
				if (playerEnt && scentEntity1 && scentEntity2 && scentEntity3 && scentEntity4 
					&& scentEntity5 && scentEntity6 && timerTextEntity && TimeRushBGM && AggroDog && CorgiSniff)
				{
					break;
				}
			}
		}
		g_Window->HideMouseCursor();
	}

	void InitLevel() override
	{
		cameraController = new CameraController(playerEnt);
		g_Checklist.OnInitialize();
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do5");
		g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.0f, 0.0f), "");

		g_Checklist.ChangeAsset(g_Checklist.Box2, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box3, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box4, glm::vec2(0.0f, 0.0f), "");
		
		if (g_Coordinator.HaveComponent<UIComponent>(g_Checklist.Paper))
		{
			g_Coordinator.GetComponent<UIComponent>(g_Checklist.Paper).set_position(glm::vec2(-0.73f, 1.165f));
		}

		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());
	}

	void UpdateLevel(double deltaTime) override
	{
		pauseLogic::OnUpdate();

		if (!g_IsPaused)
		{
			cameraController->Update(static_cast<float>(deltaTime));

			timer += deltaTime;
			cooldownTimer += deltaTime;

			if (!g_Coordinator.HaveComponent<UIComponent>(timerTextEntity)) return;

			auto& text = g_Coordinator.GetComponent<UIComponent>(timerTextEntity);

			auto& opacity1 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity1);
			auto& opacity2 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity2);
			auto& opacity3 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity3);
			auto& opacity4 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity4);
			auto& opacity5 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity5);
			auto& opacity6 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity6);

			// Change the texture every second
			if (timer >= interval && currentTextureIndex <= 233)
			{
				std::string nextTextureName = "Group" + std::to_string(currentTextureIndex + 1);

				//int nextTextureId = g_ResourceManager.GetTextureDDS(nextTextureName);
				//text.set_textureid(nextTextureId);
				text.set_texturename(nextTextureName);

				timer = 0.0; // Reset timer
				currentTextureIndex++; // Move to the next texture
			}

			// When the timer reaches the end ("Group234"), transition to the next level
			if (currentTextureIndex > 234)
			{
				std::cout << "End of timer" << std::endl;
			}

		if (g_Checklist.finishTR && g_Checklist.shutted)
		{
			/*
			auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
			if (loading)
			{
				// Pass in the name of the real scene we want AFTER the loading screen
				loading->m_NextScene = "MainHall";
				g_LevelManager.SetNextLevel("LoadingLevel");
			}
			*/
			if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && cooldownTimer >= cooldownDuration)
			{
				//	g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/CorgiSniff.wav", false, "SFX");
				if (g_Coordinator.HaveComponent<AudioComponent>(CorgiSniff)) {
					auto& music2 = g_Coordinator.GetComponent<AudioComponent>(CorgiSniff);
					music2.PlayAudio();
				}

				glm::vec4 newColor(0.3529411852359772f, 0.7058823704719544f, 0.03921568766236305f, 1.0f);
				opacity1.setParticleColor(newColor);
				opacity2.setParticleColor(newColor);
				opacity3.setParticleColor(newColor);
				opacity4.setParticleColor(newColor);
				opacity5.setParticleColor(newColor);
				opacity6.setParticleColor(newColor);

				isColorChanged = true;
				colorChangeTimer = 0.0;
				cooldownTimer = 0.0;
			}

			if (isColorChanged)
			{
				colorChangeTimer += deltaTime;
				if (colorChangeTimer >= colorChangeDuration)
				{
					glm::vec4 resetColor(0.3529411852359772f, 0.7058823704719544f, 0.03921568766236305f, 0.0f);
					opacity1.setParticleColor(resetColor);
					opacity2.setParticleColor(resetColor);
					opacity3.setParticleColor(resetColor);
					opacity4.setParticleColor(resetColor);
					opacity5.setParticleColor(resetColor);
					opacity6.setParticleColor(resetColor);

					isColorChanged = false;
				}
			}

			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);
			}

			if (g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetLastCollidedObjectName() == "WallDoor")
			{
				if (!g_Checklist.finishTR)
				{
					g_Checklist.ChangeBoxChecked(g_Checklist.Box1);
					g_Checklist.finishTR = true;
				}
			}

			if (g_Checklist.finishTR && g_Checklist.shutted)
			{
				g_LevelManager.SetNextLevel("MainHall");
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
	}

	void UnloadLevel() override
	{
		//g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/TimeRushBGM.wav");
		//g_Audio.StopBGM();

		if (g_Coordinator.HaveComponent<AudioComponent>(TimeRushBGM)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(TimeRushBGM);
			music.StopAudio();
		}

		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
		timer = 0.0;
		currentTextureIndex = 53;
		g_Checklist.finishTR = false;
	}
};
