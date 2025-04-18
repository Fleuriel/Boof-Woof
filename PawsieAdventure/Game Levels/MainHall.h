﻿#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Systems/CageBreaker/CageBreaker.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"
#include "../Systems/SmellAvoidance/SmellAvoidance.h"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../GSM/GameStateMachine.h" // for g_IsPaused


class MainHall : public Level
{
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{}, scentEntity1{}, scentEntity2{}, scentEntity3{}, puppy1{}, puppy2{}, puppy3{}, rex{}, FireSound{};

	// Smell Avoidance
	Entity pee1{}, pee2{}, pee3{}, pee4{}, pee5{}, pee6{}, pee7{}, pee8{}, pee9{}, pee10{}, pee11{}, pee12{}, pee13{}, pee14{}, pee15{},
		pee16{}, pee17{}, pee18{}, pee19{};

	Entity pee1Collider{}, pee2Collider{}, pee3Collider{}, pee4Collider{}, pee5Collider{}, pee6Collider{}, pee7Collider{}, pee8Collider{},
		pee9Collider{}, pee10Collider{}, pee11Collider{}, pee12Collider{}, pee13Collider{}, pee14Collider{}, pee15Collider{}, pee16Collider{},
		pee17Collider{}, pee18Collider{}, pee19Collider{};

	Entity peeScent1{}, peeScent2{}, peeScent3{}, peeScent4{}, peeScent5{}, peeScent6{}, peeScent7{}, peeScent8{}, peeScent9{},
		peeScent16{}, peeScent17{}, peeScent18{}, peeScent19{}, peeScent20{},
		peeScent21{}, peeScent22{}, peeScent23{}, peeScent24{}, peeScent25{};
	Entity WaterBucket{}, WaterBucket2{}, WaterBucket3{}, WaterBucket4{}, WaterBucket5{};

	Entity Cage1{}, Cage1Collider{}, Cage2{}, Cage2Collider{}, Cage3{}, Cage3Collider{};
	bool cage1Collided{ false }, cage2Collided{ false }, cage3Collided{ false };

	Entity stealthCollider1{}, stealthCollider2{}, stealthCollider3{}, stealthCollider4{}, stealthCollider5{}, stealthCollider6{};
	Entity VFXBG{}, VFX1{}, VFX2{};
	float VFX1Dir{ -0.005f }, VFX2Dir{ -0.005f };

	// Existing member variables...
	float originalBrightness = 1.0f;

	// Camera
	CameraController* cameraController = nullptr;
	bool savedcamdir{ false };
	bool teb_last = false;
	glm::vec3 camdir{};

	// Smell Avoidance
	double timer{ 0.0 }, timerLimit{ 15.0 }, timesUp{ 2.0 }; // Timer for smell avoidance
	double colorChangeTimer{ 0.0 }, colorChangeDuration{ 3.0 }, cooldownTimer{ 10.0 }, cooldownDuration{ 10.0 };
	bool isColorChanged{ false }, sniffa{ false };

	// Puppies
	int puppiesCollected = 0;
	const int puppiesNode[4] = { 11, 29, 38, 0 };
	bool collectedPuppy1{ false }, collectedPuppy2{ false }, collectedPuppy3{ false }, chgChecklist{ false };
	bool puppy1Collided{ false }, puppy2Collided{ false }, puppy3Collided{ false };
	bool dialogueFirst{ false }, dialogueSecond{ false }, dialogueThird{ false };

	double sniffCooldownTimer = 0.0;  // Timer for sniff cooldown
	const double sniffCooldownDuration = 17.0;  // 17 seconds cooldown
	bool isSniffOnCooldown = false;  // Track cooldown state
	bool bark = false;
	std::vector<Entity> particleEntities;
	std::vector<Entity> peeEntities;
	std::vector<Entity> peeColliders;
	std::vector<Entity> waterBuckets;
	std::vector<std::string> bitingSounds = {
	"Corgi/DogBite_01.wav",
	"Corgi/DogBite_02.wav",
	"Corgi/DogBite_03.wav",
	"Corgi/DogBite_04.wav",
	"Corgi/DogBite_05.wav",
	"Corgi/DogBite_06.wav",
	"Corgi/DogBite_07.wav",
	};

	float defaultVolume = g_Audio.GetSFXVolume() * g_Audio.GetMasterVolume();

	// Reverse transition state variables (for level start)
	bool reverseTransitionActive = true;
	float reverseTransitionTimer = 0.0f;
	const float reverseTransitionDuration = 1.0f; // Duration for reverse transition



	// Function to get a random sound from a vector
	std::string GetRandomSound(const std::vector<std::string>& soundList) {
		static std::random_device rd;
		static std::mt19937 gen(rd()); // Mersenne Twister PRNG
		std::uniform_int_distribution<std::size_t> dis(0, soundList.size() - 1);
		return soundList[dis(gen)];
	}

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/MainHallM6.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav", true, "BGM");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ambienceSFX.wav", true, "SFX");


		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction = GetNameToActionMap();

		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<UIComponent>(entity)) {
				auto& UIComp = g_Coordinator.GetComponent<UIComponent>(entity);
				if (UIComp.get_texturename() == "WashingVFX")
					VFXBG = entity;
				else if (UIComp.get_texturename() == "Stinky")
					VFX1 = entity;
				else if (UIComp.get_texturename() == "StinkyDog")
					VFX2 = entity;
			}

			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
				auto it = nameToAction.find(metadata.GetName());

				if (it != nameToAction.end())
				{
					it->second(entity);
				}

				// Exit early if all entities are found
				if (AllEntitiesInitialized())
				{
					break;
				}
			}
		}
		g_Player = playerEnt;
		g_Window->HideMouseCursor();
	}

	void InitLevel() override
	{
		ResetLevelState();

		// Activate reverse transition at level start.
		reverseTransitionActive = true;
		reverseTransitionTimer = 0.0f;

		cameraController = new CameraController(playerEnt);
		g_CageBreaker = CageBreaker(playerEnt, Cage1, Cage2, Cage3, Cage1Collider, Cage2Collider, Cage3Collider, puppy1, puppy2, puppy3);
		g_RopeBreaker = RopeBreaker(playerEnt, RopeEnt, RopeEnt2, BridgeEnt);

		// Smell Avoidance
		peeEntities = { pee1, pee2, pee3, pee4, pee5, pee6, pee7, pee8, pee9, pee10, pee11, pee12, pee13, pee14, pee15,
			pee16, pee17, pee18, pee19 };

		peeColliders = { pee1Collider, pee2Collider, pee3Collider, pee4Collider, pee5Collider, pee6Collider, pee7Collider, pee8Collider,
			pee9Collider, pee10Collider, pee11Collider, pee12Collider, pee13Collider, pee14Collider, pee15Collider, pee16Collider, pee17Collider,
			pee18Collider, pee19Collider };

		waterBuckets = { WaterBucket, WaterBucket2, WaterBucket3, WaterBucket4, WaterBucket5 };
		g_SmellAvoidance = SmellAvoidance(playerEnt, peeEntities, peeColliders, waterBuckets);

		g_Checklist.OnInitialize();
		InitializeChecklist();
		InitializeFireSound();
		if (g_Coordinator.HaveComponent<AudioComponent>(rex)) {
			auto& rexAudio = g_Coordinator.GetComponent<AudioComponent>(rex);
			rexAudio.SetAudioSystem(&g_Audio);

			// 🔊 Play 3D spatial sound for Rex (looped idle bark or breathing sound)
			g_Audio.PlayEntity3DAudio(rex, FILEPATH_ASSET_AUDIO + "/Rex_Growl_Loop_v1.wav", true, "BGM");
		}
		

		g_SmellAvoidance.Initialize();

		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());

		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::INMAINHALL);

		g_Coordinator.GetSystem<LogicSystem>()->ReInit();

		particleEntities = { scentEntity1, scentEntity2, scentEntity3, peeScent1, peeScent2, peeScent3, peeScent4, peeScent5, peeScent6,
			peeScent7, peeScent8, peeScent9, peeScent16, peeScent17,
			peeScent18, peeScent19, peeScent20, peeScent21, peeScent22, peeScent23, peeScent24, peeScent25 };

		g_UI.OnInitialize();
		// Store the original brightness value
		originalBrightness = g_Coordinator.GetSystem<GraphicsSystem>()->GetBrightness();
		g_UI.finishCaged = false;

		g_Coordinator.GetComponent<UIComponent>(VFXBG).set_opacity(0);

		g_Coordinator.GetComponent<UIComponent>(VFX1).set_opacity(1);
		g_Coordinator.GetComponent<UIComponent>(VFX1).set_scale(glm::vec2{ 0,0 });
		g_Coordinator.GetComponent<UIComponent>(VFX2).set_opacity(1);
		g_Coordinator.GetComponent<UIComponent>(VFX2).set_scale(glm::vec2{ 0,0 });

	}

	void UpdateLevel(double deltaTime) override
	{


		// --- Reverse Transition Effect at Level Start ---
		if (reverseTransitionActive)
		{
			g_Input.LockInput();
			reverseTransitionTimer += static_cast<float>(deltaTime);
			float revProgress = reverseTransitionTimer / reverseTransitionDuration;
			if (revProgress > 1.0f) revProgress = 1.0f;
			// Call the reverse transition effect from GraphicsSystem.
			g_Coordinator.GetSystem<GraphicsSystem>()->RenderReverseTransitionEffect(revProgress);
			if (reverseTransitionTimer >= reverseTransitionDuration)
			{
				g_Input.UnlockInput();
				reverseTransitionActive = false;
			}
		}
		else {
			g_Input.UnlockInput();
		}
		// --- End Reverse Transition ---

		if (g_IsPaused && !savedcamdir) {
			camdir = cameraController->GetCameraDirection(g_Coordinator.GetComponent<CameraComponent>(playerEnt));
			savedcamdir = true;
		}

		if (!g_IsPaused && savedcamdir) {
			cameraController->SetCameraDirection(g_Coordinator.GetComponent<CameraComponent>(playerEnt), camdir);
			savedcamdir = false;
		}
		glm::vec3 playerPos;
		if (g_Coordinator.HaveComponent<TransformComponent>(playerEnt)) {
			auto& playerTransform = g_Coordinator.GetComponent<TransformComponent>(playerEnt);
			playerPos = playerTransform.GetPosition();
			glm::vec3 playerRot = playerTransform.GetRotation();  // Get rotation from TransformComponent

			g_Audio.SetListenerPosition(playerPos, playerRot);
		}

		// Update the positions of all 3D sounds (including the fireplace)
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
			if(g_Coordinator.HaveComponent<TransformComponent>(playerEnt))
				g_UI.Sniff(particleEntities, puppiesNode[puppiesCollected], static_cast<float>(deltaTime), g_Coordinator.GetSystem<PathfindingSystem>()->GetClosestNode(playerPos));
			else
				g_UI.Sniff(particleEntities, puppiesNode[puppiesCollected], static_cast<float>(deltaTime));
			g_DialogueText.OnUpdate(deltaTime);

			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);
			}

			g_SmellAvoidance.Update(deltaTime);

			if (CheckEntityWithPlayerCollision(rex)) {
				auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
				if (loading)
				{
					// Pass in the name of the real scene we want AFTER the loading screen
					loading->m_NextScene = "MainHall";
					g_LevelManager.SetNextLevel("LoadingLevel");
					g_TimerTR.OnShutdown();
					g_DialogueText.OnShutdown();
					g_CageBreaker.ResetCB();
					g_RopeBreaker.ResetRB();
					g_BoneCatcher.ResetBC();
				}
			}
			if (!collectedPuppy1 || !collectedPuppy2 || !collectedPuppy3)
			{
				g_CageBreaker.OnUpdate(deltaTime);
				CheckPuppyCollision();
			}

			auto& VFXBG_UICOMP = g_Coordinator.GetComponent<UIComponent>(VFXBG);
			auto& VFX1_UICOMP = g_Coordinator.GetComponent<UIComponent>(VFX1);
			auto& VFX2_UICOMP = g_Coordinator.GetComponent<UIComponent>(VFX2);


			if (g_SmellAvoidance.GetPeeMarked())
			{
				if (!g_SmellAvoidance.GetTimerInit()) 
				{
					g_TimerTR.OnInitialize();
					g_TimerTR.timer = timerLimit;
					g_SmellAvoidance.SetTimerInit(true);

					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", true, "SFX");
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav", false, "SFX");
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav", true, "BGM");

					g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", g_Audio.GetSFXVolume() * g_Audio.GetMasterVolume());
					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav");
				}
				g_TimerTR.OnUpdate(deltaTime);

				float timeLeft = static_cast<float>(g_TimerTR.timer);  // Get remaining time
				float maxVolume = 1.0f;   // Maximum volume factor
				float minVolume = 0.6f;   // Minimum volume factor

				// Scale volume based on time left (louder as it approaches 0)
				float newVolume = minVolume + (1.0f - (timeLeft / static_cast<float>(timerLimit))) * (maxVolume - minVolume);

				// Apply the global SFX and master volume settings:
				float adjustedVolume = newVolume * g_Audio.GetSFXVolume() * g_Audio.GetMasterVolume();

				// Update the ticking sound volume using the adjusted volume
				g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", adjustedVolume);


				// UI
				VFXBG_UICOMP.set_position({ 0.f, 1.8f });
				VFXBG_UICOMP.set_opacity(1.0f);

				VFX1_UICOMP.set_opacity((VFX1_UICOMP.get_opacity() > 0.f) ? VFX1_UICOMP.get_opacity() - 0.0025f : 0.f);
				float VFX1_XPOS = VFX1_UICOMP.get_position().x + VFX1Dir;
				if (VFX1_XPOS < -0.6 || VFX1_XPOS > -0.4)
					VFX1Dir = -VFX1Dir;
				
				float VFX1_YPOS = VFX1_UICOMP.get_position().y + 0.0025f;
				if (VFX1_YPOS > 0.5f) {
					VFX1_YPOS = -0.69f;
					VFX1_UICOMP.set_opacity(1);
					VFX1_UICOMP.set_scale({ 0.f,0.f });
				}

				if (VFX1_YPOS > -0.3f)
					VFX1_UICOMP.set_opacity((VFX1_UICOMP.get_opacity() > 0.f) ? VFX1_UICOMP.get_opacity() - 0.01f : 0.f);
				VFX1_UICOMP.set_position({ VFX1_XPOS,VFX1_YPOS });
				auto VFX1_Scaling = VFX1_UICOMP.get_scale();
				VFX1_UICOMP.set_scale(glm::vec2{ ((VFX1_Scaling.x < 0.15f) ? VFX1_Scaling.x + 0.00075f : VFX1_Scaling.x), ((VFX1_Scaling.y < 0.25f) ? VFX1_Scaling.y + 0.00125f : VFX1_Scaling.y) });

				float VFX2_XPOS = VFX2_UICOMP.get_position().x + VFX2Dir;
				if (VFX2_XPOS > 0.6 || VFX2_XPOS < 0.4)
					VFX2Dir = -VFX2Dir;

				float VFX2_YPOS = VFX2_UICOMP.get_position().y + 0.0025f;
				if (VFX2_YPOS > 0.5f) {
					VFX2_YPOS = -0.69f;
					VFX2_UICOMP.set_opacity(1);
					VFX2_UICOMP.set_scale({ 0.f,0.f });
				}

				if (VFX2_YPOS > -0.3f)
					VFX2_UICOMP.set_opacity((VFX2_UICOMP.get_opacity() > 0.f) ? VFX2_UICOMP.get_opacity() - 0.01f : 0.f);
				VFX2_UICOMP.set_position({ VFX2_XPOS,VFX2_YPOS });
				auto VFX2_Scaling = VFX2_UICOMP.get_scale();
				VFX2_UICOMP.set_scale(glm::vec2{ ((VFX2_Scaling.x < 0.15f) ? VFX2_Scaling.x + 0.00075f : VFX2_Scaling.x), ((VFX2_Scaling.y < 0.25f) ? VFX2_Scaling.y + 0.00125f : VFX2_Scaling.y) });

				if (g_TimerTR.timer == 0.0)
				{
					timesUp -= deltaTime;

					// Restore clock-ticking sound volume to its default setting
					g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", defaultVolume);

					// Now stop the specific sounds and trigger the "Times up!" sound
					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav");
					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav");
					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav");

					// Play "Times up!" sound
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Timesup.wav", false, "SFX");

					if (timesUp < 0.0)
					{
						timesUp = 2.0;
						g_SmellAvoidance.SetPeeMarked(false);
						auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
						if (loading)
						{
							// Pass in the name of the real scene we want AFTER the loading screen
							loading->m_NextScene = "MainHall";
							g_LevelManager.SetNextLevel("LoadingLevel");
							g_TimerTR.OnShutdown();
							g_DialogueText.OnShutdown();
							g_CageBreaker.ResetCB();
							g_RopeBreaker.ResetRB();
							g_BoneCatcher.ResetBC();
						}
					}
				}
			}
else
{
	// When pee is not marked, restore the clock-ticking sound volume to default
	float defaultVolume = g_Audio.GetSFXVolume() * g_Audio.GetMasterVolume();
	g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", defaultVolume);

	if (VFXBG_UICOMP.get_position().y > -1.8f)
		VFXBG_UICOMP.set_position({ 0, VFXBG_UICOMP.get_position().y - 0.02f });
	else
		VFXBG_UICOMP.set_opacity((VFXBG_UICOMP.get_opacity() > 0.f) ? VFXBG_UICOMP.get_opacity() - 0.01f : 0.f);

	VFX1_UICOMP.set_position({ VFX1_UICOMP.get_position().x, -0.69f });
	VFX1_UICOMP.set_opacity(1);
	VFX1_UICOMP.set_scale(glm::vec2{ 0, 0 });
	VFX2_UICOMP.set_position({ VFX2_UICOMP.get_position().x, -0.69f });
	VFX2_UICOMP.set_opacity(1);
	VFX2_UICOMP.set_scale(glm::vec2{ 0, 0 });
}

			// just for speed testing to rope breaker
			if (g_Input.GetKeyState(GLFW_KEY_F5) >= 1 && !g_UI.finishCaged)
			{
				collectedPuppy1 = collectedPuppy2 = collectedPuppy3 = true;
				g_BoneCatcher.puppyCollisionOrder.push_back(1);
				g_BoneCatcher.puppyCollisionOrder.push_back(2);
				g_BoneCatcher.puppyCollisionOrder.push_back(3);
				g_UI.finishCaged = true;
			}

			if (collectedPuppy1 && collectedPuppy2 && collectedPuppy3 && !chgChecklist)
			{
				g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do9");
				g_Checklist.ChangeAsset(g_Checklist.Box1, glm::vec2(0.04f, 0.06f), "Box");
				g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.0f, 0.0f), "");
				g_Checklist.ChangeAsset(g_Checklist.Box2, glm::vec2(0.0f, 0.0f), "");
				g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.0f, 0.0f), "");
				g_Checklist.ChangeAsset(g_Checklist.Box3, glm::vec2(0.0f, 0.0f), "");

				if (g_Coordinator.HaveComponent<UIComponent>(g_Checklist.Paper))
				{
					g_Coordinator.GetComponent<UIComponent>(g_Checklist.Paper).set_position(glm::vec2(-0.73f, 1.165f));
				}

				g_UI.finishCaged = true;
				chgChecklist = true;
			}

			if (chgChecklist)
			{
				g_RopeBreaker.OnUpdate(deltaTime);
			}

			if (isSniffOnCooldown)
			{
				sniffCooldownTimer += deltaTime;
				if (sniffCooldownTimer >= sniffCooldownDuration)
				{
					isSniffOnCooldown = false;  // Reset cooldown
					sniffCooldownTimer = 0.0;   // Reset timer
				}
			}

			// Take this away once u shift to script & peecontrol file
			if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && !sniffa && cooldownTimer >= cooldownDuration && !isSniffOnCooldown)
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CorgiSniff.wav", false, "SFX");

				g_SmellAvoidance.SetDefaultPeePosition();

				isSniffOnCooldown = true;  // Start cooldown
				sniffCooldownTimer = 0.0;  // Reset timer
				sniffa = true;
				isColorChanged = true;
				colorChangeTimer = 0.0;
				cooldownTimer = 0.0;
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


			if (isColorChanged)
			{
				colorChangeTimer += deltaTime;
				if (colorChangeTimer >= colorChangeDuration)
				{
					//g_SmellAvoidance.SetNewPeePosition();
					isColorChanged = false;
				}
			}

			if (g_Input.GetKeyState(GLFW_KEY_E) == 0)
			{
				sniffa = false;
			}
		}

		HandleStealthCollision();
	}

	void FreeLevel() override
	{
		if (cameraController)
		{
			delete cameraController;
			cameraController = nullptr;
		}

		g_UI.OnShutdown();
		g_TimerTR.OnShutdown();
		g_DialogueText.OnShutdown();
	}

	void UnloadLevel() override
	{
		g_Coordinator.GetSystem<GraphicsSystem>()->SetBrightness(originalBrightness);
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav");
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav");
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav");
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav");
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/MetalCage.wav");


		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			music.StopAudio();
		}

		if (g_Coordinator.HaveComponent<AudioComponent>(rex)) {
			auto& rexAudio = g_Coordinator.GetComponent<AudioComponent>(rex);
			rexAudio.StopAudio();
			std::cout << "[Rex] Audio stopped in UnloadLevel().\n";
		}

		g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();

		// Ensure RESET for game to be replayable
		g_Checklist.shutted = false;
		sniffa = collectedPuppy1 = collectedPuppy2 = collectedPuppy3 = chgChecklist = false;
		puppy1Collided = puppy2Collided = puppy3Collided = false;
		dialogueFirst = dialogueSecond = dialogueThird = false;
		g_UI.finishCaged = false;

		puppiesCollected = 0;
	}

private:
	std::unordered_map<std::string, std::function<void(Entity)>> GetNameToActionMap()
	{
		return {
			{"Player", [&](Entity entity) { playerEnt = entity; }},
			{"Rope1", [&](Entity entity) { RopeEnt = entity; }},
			{"Rope2", [&](Entity entity) { RopeEnt2 = entity; }},
			{"DrawBridge", [&](Entity entity) { BridgeEnt = entity; }},
			{"Puppy1", [&](Entity entity) { puppy1 = entity; }},
			{"Puppy2", [&](Entity entity) { puppy2 = entity; }},
			{"Puppy3", [&](Entity entity) { puppy3 = entity; }},
			{"Rex", [&](Entity entity) { rex = entity; }},
			{"ScentTrail1", [&](Entity entity) { scentEntity1 = entity; }},
			{"ScentTrail2", [&](Entity entity) { scentEntity2 = entity; }},
			{"ScentTrail3", [&](Entity entity) { scentEntity3 = entity; }},
			{"Pee1", [&](Entity entity) { pee1 = entity; }},
			{"Pee2", [&](Entity entity) { pee2 = entity; }},
			{"Pee3", [&](Entity entity) { pee3 = entity; }},
			{"Pee4", [&](Entity entity) { pee4 = entity; }},
			{"Pee5", [&](Entity entity) { pee5 = entity; }},
			{"Pee6", [&](Entity entity) { pee6 = entity; }},
			{"Pee7", [&](Entity entity) { pee7 = entity; }},
			{"Pee8", [&](Entity entity) { pee8 = entity; }},
			{"Pee9", [&](Entity entity) { pee9 = entity; }},
			{"Pee10", [&](Entity entity) { pee10 = entity; }},
			{"Pee11", [&](Entity entity) { pee11 = entity; }},
			{"Pee12", [&](Entity entity) { pee12 = entity; }},
			{"Pee13", [&](Entity entity) { pee13 = entity; }},
			{"Pee14", [&](Entity entity) { pee14 = entity; }},
			{"Pee15", [&](Entity entity) { pee15 = entity; }},
			{"Pee16", [&](Entity entity) { pee16 = entity; }},
			{"Pee17", [&](Entity entity) { pee17 = entity; }},
			{"Pee18", [&](Entity entity) { pee18 = entity; }},
			{"Pee19", [&](Entity entity) { pee19 = entity; }},
			{"Pee1Collision", [&](Entity entity) { pee1Collider = entity; }},
			{"Pee2Collision", [&](Entity entity) { pee2Collider = entity; }},
			{"Pee3Collision", [&](Entity entity) { pee3Collider = entity; }},
			{"Pee4Collision", [&](Entity entity) { pee4Collider = entity; }},
			{"Pee5Collision", [&](Entity entity) { pee5Collider = entity; }},
			{"Pee6Collision", [&](Entity entity) { pee6Collider = entity; }},
			{"Pee7Collision", [&](Entity entity) { pee7Collider = entity; }},
			{"Pee8Collision", [&](Entity entity) { pee8Collider = entity; }},
			{"Pee9Collision", [&](Entity entity) { pee9Collider = entity; }},
			{"Pee10Collision", [&](Entity entity) { pee10Collider = entity; }},
			{"Pee11Collision", [&](Entity entity) { pee11Collider = entity; }},
			{"Pee12Collision", [&](Entity entity) { pee12Collider = entity; }},
			{"Pee13Collision", [&](Entity entity) { pee13Collider = entity; }},
			{"Pee14Collision", [&](Entity entity) { pee14Collider = entity; }},
			{"Pee15Collision", [&](Entity entity) { pee15Collider = entity; }},
			{"Pee16Collision", [&](Entity entity) { pee16Collider = entity; }},
			{"Pee17Collision", [&](Entity entity) { pee17Collider = entity; }},
			{"Pee18Collision", [&](Entity entity) { pee18Collider = entity; }},
			{"Pee19Collision", [&](Entity entity) { pee19Collider = entity; }},
			{"WaterBucket", [&](Entity entity) { WaterBucket = entity; }},
			{"WaterBucket2", [&](Entity entity) { WaterBucket2 = entity; }},
			{"WaterBucket3", [&](Entity entity) { WaterBucket3 = entity; }},
			{"WaterBucket4", [&](Entity entity) { WaterBucket4 = entity; }},
			{"WaterBucket5", [&](Entity entity) { WaterBucket5 = entity; }},
			{"red particle", [&](Entity entity) { FireSound = entity; }},
			{"Cage1", [&](Entity entity) { Cage1 = entity; }},
			{"Cage1Collider", [&](Entity entity) { Cage1Collider = entity; }},
			{"Cage2", [&](Entity entity) { Cage2 = entity; }},
			{"Cage2Collider", [&](Entity entity) { Cage2Collider = entity; }},
			{"Cage3", [&](Entity entity) { Cage3 = entity; }},
			{"Cage3Collider", [&](Entity entity) { Cage3Collider = entity; }},
			{"StealthCollider1", [&](Entity entity) { stealthCollider1 = entity; }},
			{"StealthCollider2", [&](Entity entity) { stealthCollider2 = entity; }},
			{"StealthCollider3", [&](Entity entity) { stealthCollider3 = entity; }},
			{"StealthCollider4", [&](Entity entity) { stealthCollider4 = entity; }},
			{"StealthCollider5", [&](Entity entity) { stealthCollider5 = entity; }},
			{"StealthCollider6", [&](Entity entity) { stealthCollider6 = entity; }},
			{"PeeScent1", [&](Entity entity) { peeScent1 = entity; }},
			{"PeeScent2", [&](Entity entity) { peeScent2 = entity; }},
			{"PeeScent3", [&](Entity entity) { peeScent3 = entity; }},
			{"PeeScent4", [&](Entity entity) { peeScent4 = entity; }},
			{"PeeScent5", [&](Entity entity) { peeScent5 = entity; }},
			{"PeeScent6", [&](Entity entity) { peeScent6 = entity; }},
			{"PeeScent7", [&](Entity entity) { peeScent7 = entity; }},
			{"PeeScent8", [&](Entity entity) { peeScent8 = entity; }},
			{"PeeScent9", [&](Entity entity) { peeScent9 = entity; }},
			{"PeeScent16", [&](Entity entity) { peeScent16 = entity; }},
			{"PeeScent17", [&](Entity entity) { peeScent17 = entity; }},
			{"PeeScent18", [&](Entity entity) { peeScent18 = entity; }},
			{"PeeScent19", [&](Entity entity) { peeScent19 = entity; }},
			{"PeeScent20", [&](Entity entity) { peeScent20 = entity; }},
			{"PeeScent21", [&](Entity entity) { peeScent21 = entity; }},
			{"PeeScent22", [&](Entity entity) { peeScent22 = entity; }},
			{"PeeScent23", [&](Entity entity) { peeScent23 = entity; }},
			{"PeeScent24", [&](Entity entity) { peeScent24 = entity; }},
			{"PeeScent25", [&](Entity entity) { peeScent25 = entity; }},
		};
	}

	bool AllEntitiesInitialized() const
	{
		return playerEnt && RopeEnt && RopeEnt2 && BridgeEnt && puppy1 && puppy2 && puppy3 && scentEntity1 && scentEntity2 && scentEntity3 && stealthCollider6;
	}

	void InitializeChecklist()
	{
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do6");
		g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.15f, 0.05f), "Do7");
		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.15f, 0.05f), "Do8");
		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.0f, 0.0f), "");
		g_Checklist.ChangeAsset(g_Checklist.Box4, glm::vec2(0.0f, 0.0f), "");

		if (g_Coordinator.HaveComponent<UIComponent>(g_Checklist.Paper))
		{
			g_Coordinator.GetComponent<UIComponent>(g_Checklist.Paper).set_position(glm::vec2(-0.73f, 0.968f));
		}
	}

	void InitializeFireSound() const
	{
		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& fireAudio = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			fireAudio.SetAudioSystem(&g_Audio);
			g_Audio.PlayEntity3DAudio(FireSound, FILEPATH_ASSET_AUDIO + "/Fire.wav", true, "BGM");
			std::cout << " Fire Sound initialized in InitLevel for entity " << FireSound << std::endl;
		}
		else {
			std::cerr << " ERROR: FireSound entity has no AudioComponent in InitLevel!" << std::endl;
		}
	}

	bool CheckEntityWithPlayerCollision(Entity entity) const
	{
		//Check Entity Collision with Player
		if (g_Coordinator.HaveComponent<CollisionComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
		{
			auto& collider1 = g_Coordinator.GetComponent<CollisionComponent>(entity);
			if (collider1.GetIsColliding() && std::strcmp(collider1.GetLastCollidedObjectName().c_str(), "Player") == 0)
				return true;
		}
		return false;
	}

	bool CheckRexCollision()
	{
		return CheckEntityWithPlayerCollision(rex);
	}

	void CheckPuppyCollision()
	{
		puppy1Collided = CheckEntityWithPlayerCollision(puppy1);
		puppy2Collided = CheckEntityWithPlayerCollision(puppy2);
		puppy3Collided = CheckEntityWithPlayerCollision(puppy3);

		if (puppy1Collided && !collectedPuppy1)
		{
			puppiesCollected++;
			g_Checklist.ChangeBoxChecked(g_Checklist.Box1);
			collectedPuppy1 = true;
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Corgi/SmallDogBark1.wav", false, "SFX");
			g_BoneCatcher.puppyCollisionOrder.push_back(1);
		}

		if (puppy2Collided && !collectedPuppy2)
		{
			puppiesCollected++;
			g_Checklist.ChangeBoxChecked(g_Checklist.Box2);
			collectedPuppy2 = true;
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Corgi/SmallDogBark2.wav", false, "SFX");
			g_BoneCatcher.puppyCollisionOrder.push_back(2);
		}

		if (puppy3Collided && !collectedPuppy3)
		{
			puppiesCollected++;
			g_Checklist.ChangeBoxChecked(g_Checklist.Box3);
			collectedPuppy3 = true;
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Corgi/SmallDogBark3.wav", false, "SFX");
			g_BoneCatcher.puppyCollisionOrder.push_back(3);
		}

		// Update the checklist and dialogue based on the number of puppies collected

		if (puppiesCollected == 1 && !dialogueFirst)
		{
			g_DialogueText.OnInitialize();
			g_DialogueText.setDialogue(DialogueState::TWOMORETOGO);
			dialogueFirst = true;
		}
		else if (puppiesCollected == 2 && !dialogueSecond)
		{
			g_DialogueText.OnInitialize();
			g_DialogueText.setDialogue(DialogueState::ONEMORETOGO);
			dialogueSecond = true;
		}
		else if (puppiesCollected == 3 && !dialogueThird)
		{
			g_DialogueText.OnInitialize();
			g_DialogueText.setDialogue(DialogueState::ALLPUPSFOUND);
			dialogueThird = true;
		}
	}

	void HandleStealthCollision()
	{
		bool isColliding = CheckEntityWithPlayerCollision(stealthCollider1) ||
			CheckEntityWithPlayerCollision(stealthCollider2) ||
			CheckEntityWithPlayerCollision(stealthCollider3) ||
			CheckEntityWithPlayerCollision(stealthCollider4) ||
			CheckEntityWithPlayerCollision(stealthCollider5) ||
			CheckEntityWithPlayerCollision(stealthCollider6);

		static bool wasColliding = false;

		if (isColliding)
		{
			// Decrease the brightness of the lights
			float currentBrightness = g_Coordinator.GetSystem<GraphicsSystem>()->GetBrightness();
			float newBrightness = std::max(0.5f, currentBrightness - 0.05f); // Decrease by 0.1, but not below 0
			g_Coordinator.GetSystem<GraphicsSystem>()->SetBrightness(newBrightness);
			wasColliding = true;
			g_Coordinator.GetSystem<LogicSystem>()->isPlayerhidden = true;
		}
		else if (wasColliding)
		{
			// Reset the brightness to the original value
			g_Coordinator.GetSystem<GraphicsSystem>()->SetBrightness(originalBrightness);
			wasColliding = false;
			g_Coordinator.GetSystem<LogicSystem>()->isPlayerhidden = false;
		}
	}
	void ResetLevelState()
	{
		// Reset collision flags for cages
		cage1Collided = false;
		cage2Collided = false;
		cage3Collided = false;

		// Reset camera-related variables
		savedcamdir = false;
		teb_last = false;
		camdir = glm::vec3(0.0f);

		// Reset smell avoidance timers/flags
		timer = 0.0;
		timerLimit = 15.0;
		timesUp = 2.0;
		colorChangeTimer = 0.0;
		colorChangeDuration = 3.0;
		cooldownTimer = 10.0;
		cooldownDuration = 10.0;
		isColorChanged = false;
		sniffa = false;

		// Reset puppy collection state
		puppiesCollected = 0;
		collectedPuppy1 = false;
		collectedPuppy2 = false;
		collectedPuppy3 = false;
		chgChecklist = false;
		puppy1Collided = false;
		puppy2Collided = false;
		puppy3Collided = false;
		dialogueFirst = false;
		dialogueSecond = false;
		dialogueThird = false;

		// Reset sniff cooldown
		sniffCooldownTimer = 0.0;
		isSniffOnCooldown = false;

		// Reset particle effects and bark flag
		particleEntities.clear();
		bark = false;
	}
};