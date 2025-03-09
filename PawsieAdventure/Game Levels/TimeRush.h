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
	Entity rexEnt{};
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

	double sniffCooldownTimer = 0.0;  // Accumulates time
	const double sniffCooldownDuration = 17.0;  // 16 seconds
	bool isSniffOnCooldown = false;

	std::vector<Entity> particleEntities;

	bool bark = false;
	std::vector<std::string> bitingSounds = {
	"Corgi/DogBite_01.wav",
	"Corgi/DogBite_02.wav",
	"Corgi/DogBite_03.wav",
	"Corgi/DogBite_04.wav",
	"Corgi/DogBite_05.wav",
	"Corgi/DogBite_06.wav",
	"Corgi/DogBite_07.wav",
		};


	// Function to get a random sound from a vector
	std::string GetRandomSound(const std::vector<std::string>& soundList) {
		static std::random_device rd;
		static std::mt19937 gen(rd()); // Mersenne Twister PRNG
		std::uniform_int_distribution<std::size_t> dis(0, soundList.size() - 1);
		return soundList[dis(gen)];
	}

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/TimeRushPuzzle.json");
		g_TimerTR.OnInitialize();

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ambienceSFX.wav", true, "SFX");


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
			{ "red particle", [&](Entity entity) { FireSound = entity; }},
			{"Rex", [&](Entity entity) { rexEnt = entity; }}

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
					&& scentEntity5 && scentEntity6 && scentEntity7 && scentEntity8 && scentEntity9 && TimeRushBGM && AggroDog && CorgiSniff && FireSound && rexEnt)
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

		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::ENTEREDLIBRARY);

		particleEntities = { scentEntity1, scentEntity2, scentEntity3, scentEntity4, scentEntity5, scentEntity6, scentEntity7, scentEntity8, scentEntity9 };
		g_UI.OnInitialize();
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

		if (!g_DialogueText.dialogueActive) 
		{
			pauseLogic::OnUpdate();
		}

		if (!g_IsPaused)
		{
			cameraController->Update(static_cast<float>(deltaTime));
			cooldownTimer += deltaTime;

			g_UI.OnUpdate(static_cast<float>(deltaTime));
			g_UI.Sniff(particleEntities, static_cast<float>(deltaTime));

			g_TimerTR.OnUpdate(deltaTime);

			g_DialogueText.checkCollision(playerEnt, deltaTime);
			g_DialogueText.OnUpdate(deltaTime);

			// Player lost, sent back to starting point -> checklist doesn't need to reset since it means u nvr clear the level.
			if (g_TimerTR.timer == 0.0) 
			{
				/*
				// Need to teleport Rex to 20, 1.5, 3
				if (g_Coordinator.HaveComponent<TransformComponent>(rexEnt))
				{
					auto& rexTransform = g_Coordinator.GetComponent<TransformComponent>(rexEnt);
					rexTransform.SetPosition(glm::vec3(20.0f, 1.5f, 3.0f));
				}
				*/
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
						g_DialogueText.OnShutdown();
						g_DialogueText.Reset();

						g_LevelManager.SetNextLevel("LoadingLevel");
					}
					
				}
			}
			
			//if (CheckEntityWithPlayerCollision(rexEnt)) {
			//	auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
			//	if (loading)
			//	{
			//		// Pass in the name of the real scene we want AFTER the loading screen
			//		loading->m_NextScene = "TimeRush";

			//		timesUp = 2.0;
			//		g_TimerTR.Reset();
			//		g_DialogueText.OnShutdown();
			//		g_DialogueText.Reset();

			//		g_LevelManager.SetNextLevel("LoadingLevel");
			//	}
			//}
			
			// Take this away once u shift to script
			// Accumulate time for cooldown
			if (isSniffOnCooldown) {
				sniffCooldownTimer += deltaTime;
				if (sniffCooldownTimer >= sniffCooldownDuration) {
					isSniffOnCooldown = false;  // Reset cooldown
					sniffCooldownTimer = 0.0;   // Reset timer
				}
			}

			// Sniffing logic with cooldown check
			if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && !isSniffOnCooldown)
			{
				if (g_Coordinator.HaveComponent<AudioComponent>(CorgiSniff))
				{
					auto& sniffSound = g_Coordinator.GetComponent<AudioComponent>(CorgiSniff);
					sniffSound.PlayAudio();
				}

				isSniffOnCooldown = true;  // Start cooldown
				sniffCooldownTimer = 0.0;  // Reset timer to start counting 16 seconds
			}




			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 1 && !bark)
			{
				std::string biteSound = FILEPATH_ASSET_AUDIO + "/" + GetRandomSound(bitingSounds);
				g_Audio.PlayFileOnNewChannel(biteSound.c_str(), false, "SFX");

				bark = true;

				// Reset bark after a short delay to allow multiple bites
				std::thread([&]() {
					std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 500ms delay
					bark = false;
					}).detach();
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

		g_UI.OnShutdown();
	}

	void UnloadLevel() override
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/Timesup.wav");
		//g_Audio.StopBGM();
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ambienceSFX.wav");


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

private:

	bool CheckEntityWithPlayerCollision(Entity entity)
	{
		//Check Entity Collision with Player
		if (g_Coordinator.HaveComponent<CollisionComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
		{
			auto collider1 = g_Coordinator.GetComponent<CollisionComponent>(entity);
			if (collider1.GetIsColliding() && std::strcmp(collider1.GetLastCollidedObjectName().c_str(), "Player") == 0)
				return true;
		}
		return false;
	}
};
