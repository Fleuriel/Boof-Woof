#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/BoneCatcher/BoneCatcher.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"
#include "../Systems/SmellAvoidance/SmellAvoidance.h"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../GSM/GameStateMachine.h" // for g_IsPaused


class MainHall : public Level
{
	Entity playerEnt{}, RopeEnt{}, RopeEnt2{}, BridgeEnt{}, scentEntity1{}, scentEntity2{}, scentEntity3{}, puppy1{}, puppy2{}, puppy3{};
	Entity WaterBucket{}, WaterBucket2{}, WaterBucket3{}; // Smell Avoidance
	Entity FireSound{};

	Entity TestPee{}, TestCollider{}; // Smell Avoidance
	Entity pee1{}, pee2{}, pee3{}, pee4{}; // Smell Avoidance
	Entity pee1Collider{}, pee2Collider{}, pee3Collider{}, pee4Collider{}; // Smell Avoidance

	// Camera
	CameraController* cameraController = nullptr;
	bool savedcamdir{ false };
	bool teb_last = false;
	glm::vec3 camdir{};

	// Smell Avoidance
	double timer{ 0.0 }, timerLimit{ 10.0 }, timesUp{ 2.0 }; // Timer for smell avoidance
	double colorChangeTimer{ 0.0 }, colorChangeDuration{ 3.0 }, cooldownTimer{ 10.0 }, cooldownDuration{ 10.0 };
	bool isColorChanged{false}, sniffa{ false };

	// Puppies
	bool collectedPuppy1{ false }, collectedPuppy2{ false }, collectedPuppy3{ false }, chgChecklist{ false };
	int puppiesCollected = 0;
	bool puppy1Collided{ false }, puppy2Collided{ false }, puppy3Collided{ false };
	bool dialogueFirst{ false }, dialogueSecond{ false }, dialogueThird{ false };

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
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/MainHallM5.json");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/BedRoomMusicBGM.wav", true, "BGM");
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ambienceSFX.wav", true, "SFX");


		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction = GetNameToActionMap();

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
				if (AllEntitiesInitialized())
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
		g_RopeBreaker = RopeBreaker(playerEnt, RopeEnt, RopeEnt2, BridgeEnt);
		g_SmellAvoidance = SmellAvoidance(playerEnt, pee1, pee2, pee3, pee4, pee1Collider, pee2Collider, pee3Collider, pee4Collider, WaterBucket, WaterBucket2, WaterBucket3, TestPee, TestCollider);
		g_Checklist.OnInitialize();
		InitializeChecklist();
		InitializeFireSound();
		g_SmellAvoidance.Initialize();

		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());

		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::OUTOFLIBRARY);

		g_Coordinator.GetSystem<LogicSystem>()->ReInit();

		particleEntities = { scentEntity1, scentEntity2, scentEntity3 };
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
			g_UI.Sniff(particleEntities, static_cast<float>(deltaTime));
			g_DialogueText.OnUpdate(deltaTime);

			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);
			}

			g_SmellAvoidance.Update(deltaTime);

			if (!collectedPuppy1 || !collectedPuppy2 || !collectedPuppy3)
			{
				CheckPuppyCollision();
			}

			if (g_SmellAvoidance.GetPeeMarked())
			{
				if (!g_SmellAvoidance.GetTimerInit()) {
					g_TimerTR.OnInitialize();
					g_TimerTR.timer = timerLimit;
					g_SmellAvoidance.SetTimerInit(true);

					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", true, "SFX");
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav", false, "SFX");
					g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav", true, "BGM");


					g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", 0.4f);
					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav");


				}
				g_TimerTR.OnUpdate(deltaTime);

				// Gradually Increase Clock Ticking Volume (Only for this sound)
				float timeLeft = static_cast<float>(g_TimerTR.timer);  // Get remaining time
				float maxVolume = 1.0f;  // 100% Volume
				float minVolume = 0.6f;  // Starting Volume (20%)

				// Scale volume based on time left (louder as it approaches 0)
				float newVolume = minVolume + (1.0f - (timeLeft / static_cast<float>(timerLimit))) * (maxVolume - minVolume);

				// Update the ticking sound volume
				g_Audio.SetSoundVolume(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav", newVolume);

				if (g_TimerTR.timer == 0.0)
				{
					timesUp -= deltaTime;

					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ClockTicking_Loop.wav");
					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/GameOver_Hit 1.wav");			
					g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/Music_Danger_Loop.wav");


					// Times up! sound
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
						}
					}
				}
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

				chgChecklist = true;
			}

			if (chgChecklist)
			{
				g_RopeBreaker.OnUpdate(deltaTime);
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

			// Take this away once u shift to script & peecontrol file
			if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && !sniffa && cooldownTimer >= cooldownDuration)
			{
				g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CorgiSniff.wav", false, "SFX");

				//SetDefaultPeePosition();
				g_SmellAvoidance.SetDefaultPeePosition();

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
					//SetNewPeePosition();
					g_SmellAvoidance.SetNewPeePosition();

					isColorChanged = false;
				}
			}

			if (g_Input.GetKeyState(GLFW_KEY_E) == 0)
			{
				sniffa = false;
			}
			// until here
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
		g_TimerTR.OnShutdown();
		g_DialogueText.OnShutdown();
	}

	void UnloadLevel() override
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/BedRoomMusicBGM.wav");
		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			music.StopAudio();
		}
		g_Audio.StopBGM();
		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();

		// Ensure RESET for game to be replayable
		g_Checklist.shutted = false;
		sniffa = collectedPuppy1 = collectedPuppy2 = collectedPuppy3 = chgChecklist = false;
		//playercollided = false;
		puppy1Collided = puppy2Collided = puppy3Collided = false;
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
			{"ScentTrail1", [&](Entity entity) { scentEntity1 = entity; }},
			{"ScentTrail2", [&](Entity entity) { scentEntity2 = entity; }},
			{"ScentTrail3", [&](Entity entity) { scentEntity3 = entity; }},
			{"Pee1", [&](Entity entity) { pee1 = entity; }},
			{"Pee2", [&](Entity entity) { pee2 = entity; }},
			{"Pee3", [&](Entity entity) { pee3 = entity; }},
			{"Pee4", [&](Entity entity) { pee4 = entity; }},
			{"Pee1Collision", [&](Entity entity) { pee1Collider = entity; }},
			{"Pee2Collision", [&](Entity entity) { pee2Collider = entity; }},
			{"Pee3Collision", [&](Entity entity) { pee3Collider = entity; }},
			{"Pee4Collision", [&](Entity entity) { pee4Collider = entity; }},
			{"WaterBucket", [&](Entity entity) { WaterBucket = entity; }},
			{"WaterBucket2", [&](Entity entity) { WaterBucket2 = entity; }},
			{"WaterBucket3", [&](Entity entity) { WaterBucket3 = entity; }},
			{"red particle", [&](Entity entity) { FireSound = entity; }},
			{"TestObject", [&](Entity entity) { TestPee = entity; }},
			{"TestCollision", [&](Entity entity) { TestCollider = entity; }}
		};
	}

	bool AllEntitiesInitialized() const
	{
		return playerEnt && RopeEnt && RopeEnt2 && BridgeEnt && puppy1 && puppy2 && puppy3 && scentEntity1 && scentEntity2 && scentEntity3
			&& pee1 && pee2 && pee3 && pee4 && pee1Collider && pee2Collider && pee3Collider && pee4Collider && WaterBucket && WaterBucket2 && WaterBucket3 && TestPee && TestCollider;
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

	bool CheckEntityWithPlayerCollision(Entity entity)
	{
		//Check Entity Collision with Player
		if (g_Coordinator.HaveComponent<CollisionComponent>(entity) && g_Coordinator.HaveComponent<CollisionComponent>(playerEnt))
		{
			auto collider1 = g_Coordinator.GetComponent<CollisionComponent>(entity);
			if(collider1.GetIsColliding() && std::strcmp(collider1.GetLastCollidedObjectName().c_str(), "Player") == 0)
				return true;
		}
		return false;
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


		}

		if (puppy2Collided && !collectedPuppy2)
		{
			puppiesCollected++;
			std::cout << "hello\n" << std::endl;
			g_Checklist.ChangeBoxChecked(g_Checklist.Box2);
			collectedPuppy2 = true;
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Corgi/SmallDogBark2.wav", false, "SFX");

		}

		if (puppy3Collided && !collectedPuppy3)
		{
			puppiesCollected++;
			g_Checklist.ChangeBoxChecked(g_Checklist.Box3);
			collectedPuppy3 = true;
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/Corgi/SmallDogBark3.wav", false, "SFX");

		}

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
};