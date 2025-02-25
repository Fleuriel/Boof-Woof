#pragma once
#include "Level Manager/Level.h"
#include "ResourceManager/ResourceManager.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/TimerTR/TimerTR.h"
#include "../GSM/GameStateMachine.h" // for g_IsPaused

class TimeRush : public Level
{
	Entity playerEnt{};
	Entity scentEntity1{}, scentEntity2{}, scentEntity3{}, scentEntity4{}, scentEntity5{}, scentEntity6{}, scentEntity7{}, scentEntity8{}, scentEntity9{};
	CameraController* cameraController = nullptr;
	bool savedcamdir{ false };
	glm::vec3 camdir{};

	Entity TimeRushBGM{}, AggroDog{}, CorgiSniff{}, FireSound{};

	double colorChangeTimer = 0.0;
	double colorChangeDuration = 3.0; // Duration for which the color change lasts
	double cooldownTimer = 10.0;
	double cooldownDuration = 10.0; // Cooldown duration
	bool isColorChanged = false;

	bool finishTR{ false };
	double timesUp = 2.0;

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/TimeRushPuzzle.json");
		g_TimerTR.OnInitialize();
		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::ENTEREDLIBRARY);

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
			{"ScentTrail7", [&](Entity entity) { scentEntity7 = entity; }},
			{"ScentTrail8", [&](Entity entity) { scentEntity8 = entity; }},
			{"ScentTrail9", [&](Entity entity) { scentEntity9 = entity; }},
			{"TimeRushBGM", [&](Entity entity) { TimeRushBGM = entity; }},
			{"AggressiveDogBarking", [&](Entity entity) { AggroDog = entity; }},
			{"CorgiSniff", [&](Entity entity) { CorgiSniff = entity; }},
			{ "red particle", [&](Entity entity) { FireSound = entity; }}

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
				if (playerEnt && scentEntity1 && scentEntity2 && scentEntity3 && scentEntity4
					&& scentEntity5 && scentEntity6 && scentEntity7 && scentEntity8 && scentEntity9 && TimeRushBGM && AggroDog && CorgiSniff && FireSound)
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

		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& fireAudio = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			fireAudio.SetAudioSystem(&g_Audio);

			// Play Fire Audio (3D BGM)
			g_Audio.PlayEntity3DAudio(FireSound, FILEPATH_ASSET_AUDIO + "/Fire.wav", true, "BGM");
			std::cout << " Fire Sound initialized in InitLevel for entity " << FireSound << std::endl;
		}
		else {
			std::cerr << " ERROR: FireSound entity has no AudioComponent in InitLevel!" << std::endl;
		}


		if (g_Coordinator.HaveComponent<AudioComponent>(TimeRushBGM)) {
			auto& bgmAudio = g_Coordinator.GetComponent<AudioComponent>(TimeRushBGM);
			bgmAudio.SetAudioSystem(&g_Audio);
			bgmAudio.PlayAudio();
		}
		else {
			std::cerr << " ERROR: TimeRushBGM entity has no AudioComponent in InitLevel!" << std::endl;
		}

		if (g_Coordinator.HaveComponent<AudioComponent>(AggroDog)) {
			auto& dogAudio = g_Coordinator.GetComponent<AudioComponent>(AggroDog);
			dogAudio.SetAudioSystem(&g_Audio);
			dogAudio.PlayAudio();
		}
		else {
			std::cerr << " ERROR: AggroDog entity has no AudioComponent in InitLevel!" << std::endl;
		}


		if (g_Coordinator.HaveComponent<AudioComponent>(CorgiSniff)) {
			auto& dogAudio = g_Coordinator.GetComponent<AudioComponent>(CorgiSniff);
			dogAudio.SetAudioSystem(&g_Audio);
		}


		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());
	}

	void UpdateLevel(double deltaTime) override
	{

		if (g_IsPaused && !savedcamdir) {
			camdir = cameraController->GetCameraDirection(g_Coordinator.GetComponent<CameraComponent>(playerEnt));
			savedcamdir = true;
		}

		if (!g_IsPaused && savedcamdir) {
			cameraController->SetCameraDirection(g_Coordinator.GetComponent<CameraComponent>(playerEnt), camdir);
			savedcamdir = false;
		}

		if (g_Coordinator.HaveComponent<TransformComponent>(playerEnt)) {
			auto& playerTransform = g_Coordinator.GetComponent<TransformComponent>(playerEnt);
			glm::vec3 playerPos = playerTransform.GetPosition();
			glm::vec3 playerRot = playerTransform.GetRotation();  // Get rotation from TransformComponent

			g_Audio.SetListenerPosition(playerPos, playerRot);
		}


		// ?? Update the positions of all 3D sounds (including the fireplace)
		g_Audio.Update3DSoundPositions();


		pauseLogic::OnUpdate();

		if (!g_IsPaused)
		{
			cameraController->Update(static_cast<float>(deltaTime));

			cooldownTimer += deltaTime;

			auto& opacity1 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity1);
			auto& opacity2 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity2);
			auto& opacity3 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity3);
			auto& opacity4 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity4);
			auto& opacity5 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity5);
			auto& opacity6 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity6);
			auto& opacity7 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity7);
			auto& opacity8 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity8);
			auto& opacity9 = g_Coordinator.GetComponent<ParticleComponent>(scentEntity9);

			g_TimerTR.OnUpdate(deltaTime);

			g_DialogueText.checkCollision(playerEnt, deltaTime);
			g_DialogueText.OnUpdate(deltaTime);

			// Player lost, sent back to starting point -> checklist doesn't need to reset since it means u nvr clear the level.
			if (g_TimerTR.timer == 0.0) 
			{
				timesUp -= deltaTime;

				// Times up! sound
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Timesup.wav", false, "SFX");

				// Wait for like 2 seconds then restart game
				if (timesUp < 0.0) 
				{
					timesUp = 0.0;

					auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
					if (loading)
					{
						// Pass in the name of the real scene we want AFTER the loading screen
						loading->m_NextScene = "TimeRush";

						timesUp = 2.0;
						g_TimerTR.Reset();

						g_LevelManager.SetNextLevel("LoadingLevel");
					}
				}		
			}

			// Particles
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

				opacity7.setParticleColor(glm::vec4(0.2980392277240753f, 0.529411792755127f, 0.0941176488995552f, 1.0f));
				opacity8.setParticleColor(glm::vec4(0.2980392277240753f, 0.529411792755127f, 0.0941176488995552f, 1.0f));
				opacity9.setParticleColor(glm::vec4(0.2980392277240753f, 0.529411792755127f, 0.0941176488995552f, 1.0f));

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

					opacity7.setParticleColor(glm::vec4(0.2980392277240753f, 0.529411792755127f, 0.0941176488995552f, 0.0f));
					opacity8.setParticleColor(glm::vec4(0.2980392277240753f, 0.529411792755127f, 0.0941176488995552f, 0.0f));
					opacity9.setParticleColor(glm::vec4(0.2980392277240753f, 0.529411792755127f, 0.0941176488995552f, 0.0f));

					isColorChanged = false;
				}
			}

			// Checklist
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
				g_TimerTR.OnShutdown();
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
	}

	void UnloadLevel() override
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/Timesup.wav");
		//g_Audio.StopBGM();

		if (g_Coordinator.HaveComponent<AudioComponent>(TimeRushBGM)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(TimeRushBGM);
			music.StopAudio();
		}

		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			music.StopAudio();
		}

		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
		g_Checklist.finishTR = false;
	}
};
