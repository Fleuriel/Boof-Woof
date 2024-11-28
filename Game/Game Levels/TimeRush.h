#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"

class TimeRush : public Level
{
	double timer = 0.0;
	double interval = 1.0; // Time interval in seconds
	int currentTextureIndex = 53; // Start from "Group53"
	Entity timerTextEntity{}, playerEnt{}, scentEntity1{}, scentEntity2{}, scentEntity3{}, scentEntity4{}, scentEntity5{}, scentEntity6{};
	CameraController* cameraController = nullptr;

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
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/TimeRushPuzzle.json");
		g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/Timer.json");
		g_Audio.PlayBGM("../BoofWoof/Assets/Audio/TimeRushBGM.wav");
		g_Audio.PlayFileOnNewChannel("../BoofWoof/Assets/Audio/AggressiveDogBarking.wav");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Player")
				{
					playerEnt = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail1")
				{
					scentEntity1 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail2")
				{
					scentEntity2 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail3")
				{
					scentEntity3 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail4")
				{
					scentEntity4 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail5")
				{
					scentEntity5 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "ScentTrail6")
				{
					scentEntity6 = entity;
				}

				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Group")
				{
					timerTextEntity = entity;
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
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec3(0.3f, 0.09f, 0.0f), "Do5");
		g_Checklist.ChangeAsset(g_Checklist.Box2, glm::vec3(0.0f, 0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box3, glm::vec3(0.0f, 0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box4, glm::vec3(0.0f, 0.0f, 0.0f), "");
	}

	void UpdateLevel(double deltaTime) override
	{
		cameraController->Update(static_cast<float>(deltaTime));

		timer += deltaTime;
		cooldownTimer += deltaTime;

		if (!g_Coordinator.HaveComponent<GraphicsComponent>(timerTextEntity)) return;

		auto& text = g_Coordinator.GetComponent<GraphicsComponent>(timerTextEntity);

		auto& opacity1 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity1);
		auto& opacity2 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity2);
		auto& opacity3 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity3);
		auto& opacity4 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity4);
		auto& opacity5 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity5);
		auto& opacity6 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity6);

		// Change the texture every second
		if (timer >= interval && currentTextureIndex <= 233)
		{
			std::string currentTextureName = "Group" + std::to_string(currentTextureIndex);
			std::string nextTextureName = "Group" + std::to_string(currentTextureIndex + 1);

			int currentTextureId = g_ResourceManager.GetTextureDDS(currentTextureName);

			// Remove the current texture and add the next one
			if (text.RemoveTexture(currentTextureId))
			{
				text.clearTextures();

				int nextTextureId = g_ResourceManager.GetTextureDDS(nextTextureName);
				text.AddTexture(nextTextureId);
				text.setTexture(nextTextureName);
			}

			timer = 0.0; // Reset timer
			currentTextureIndex++; // Move to the next texture
		}

		// When the timer reaches the end ("Group234"), transition to the next level
		if (currentTextureIndex > 234)
		{
			std::cout << "End of timer" << std::endl;
		}

		if (g_Input.GetKeyState(GLFW_KEY_R) >= 1 && cooldownTimer >= cooldownDuration)
		{
			g_Audio.PlayFileOnNewChannel("../BoofWoof/Assets/Audio/CorgiSniff.wav");

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
		g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
		timer = 0.0;
		currentTextureIndex = 53;
		g_Checklist.finishTR = false;
	}
};
