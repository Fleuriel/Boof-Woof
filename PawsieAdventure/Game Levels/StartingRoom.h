#pragma once
#include "Level Manager/Level.h"
#include "ECS/Coordinator.hpp"
#include "../BoofWoof/Core/AssetManager/FilePaths.h"
#include "../Systems/CameraController/CameraController.h"
#include "../Systems/ChangeText/ChangeText.h"
#include "../Systems/Checklist/Checklist.h"
#include "../Systems/RopeBreaker/RopeBreaker.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"
#include "../Utilities/ForGame/UI/UI.h"
#include "LoadingLevel.h"



class StartingRoom : public Level
{
public:
	Entity playerEnt{}, RopeE{}, RopeObj{};
	CameraController* cameraController = nullptr;
	bool bark{ false }, sniff{ false }, initChecklist{ false };
	Entity BedRoomBGM{}, CorgiBark{}, CorgiSniff{}, FireSound{};

	bool savedcamdir{ false };
	glm::vec3 camdir{};

	std::vector<Entity> particleEntities;
	const int goalnode = 8 ;
	double sniffCooldownTimer = 0.0;  // Accumulates time
	const double sniffCooldownDuration = 17.0;  // 17 seconds
	bool isSniffOnCooldown = false;
	bool destroyedRope{ false };

	// Reverse transition state variables (for level start)
	bool reverseTransitionActive = true;
	float reverseTransitionTimer = 0.0f;
	const float reverseTransitionDuration = 1.0f; // Duration for reverse transition

	std::vector<std::string> bitingSounds = {
	"Corgi/DogBite_01.wav",
	"Corgi/DogBite_02.wav",
	"Corgi/DogBite_03.wav",
	"Corgi/DogBite_04.wav",
	"Corgi/DogBite_05.wav",
	"Corgi/DogBite_06.wav",
	"Corgi/DogBite_07.wav",
	};

	// Transition state variables
	bool transitionActive = false;
	float transitionTimer = 0.0f;
	const float transitionDuration = 1.0f; // Duration in seconds


	// Function to get a random sound from a vector
	std::string GetRandomSound(const std::vector<std::string>& soundList) {
		static std::random_device rd;
		static std::mt19937 gen(rd()); // Mersenne Twister PRNG
		std::uniform_int_distribution<std::size_t> dis(0, soundList.size() - 1);
		return soundList[dis(gen)];
	}

	void LoadLevel() override
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/StartingRoom_Light.json");
		g_DialogueText.OnInitialize();
		g_DialogueText.setDialogue(DialogueState::TUTORIALSTART);
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/ambienceSFX.wav", true, "SFX");

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		// Use unordered_map to make it O(1) efficiency
		std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
		{
			{"Player", [&](Entity entity) { playerEnt = entity; }},
			{"BedRoomMusic", [&](Entity entity) { BedRoomBGM = entity; }},
			{"CorgiBark1", [&](Entity entity) { CorgiBark = entity; }},
			{"CorgiSniff", [&](Entity entity) { CorgiSniff = entity; }},
			{"middle particle", [&](Entity entity) { FireSound = entity; }},
			{"Rope", [&](Entity entity) { RopeE = entity; }},
			{"RopeObject", [&](Entity entity) { RopeObj = entity; }}
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

				if (g_Coordinator.HaveComponent<AudioComponent>(entity))
				{
					auto& fire = g_Coordinator.GetComponent<AudioComponent>(entity);
					fire.SetAudioSystem(&g_Audio);

					if (metadata.GetName() == "middle particle")
					{
						g_Audio.PlayEntity3DAudio(FireSound, FILEPATH_ASSET_AUDIO + "/Fire.wav", true, "BGM");
					}
					else {
						std::cerr << "? ERROR: Fireplace entity has no AudioComponent!" << std::endl;
					}
				}

				// Exit early if all entities are found
				if (playerEnt && BedRoomBGM && CorgiBark && CorgiSniff && FireSound && RopeE && RopeObj)
				{
					break;
				}
			}
		}
		g_Window->HideMouseCursor();
	}

	void InitLevel() override
	{

		ResetLevelState();

		// Activate reverse transition at level start.
		reverseTransitionActive = true;
		reverseTransitionTimer = 0.0f;

		// Ensure player entity is valid
		cameraController = new CameraController(playerEnt);
		cameraController->ToggleCameraMode();

		camerachange = false;
		g_Audio.SetBGMVolume(g_Audio.GetBGMVolume());
		g_Audio.SetSFXVolume(g_Audio.GetSFXVolume());

		g_UI.OnInitialize();
		g_RopeBreaker = RopeBreaker(playerEnt, RopeE);
		g_UI.inStartingRoom = true;
		g_Player = playerEnt;
	}

	bool teb_last = false;

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

		if (g_Coordinator.HaveComponent<TransformComponent>(playerEnt)) {
			auto& playerTransform = g_Coordinator.GetComponent<TransformComponent>(playerEnt);
			glm::vec3 playerPos = playerTransform.GetPosition();
			glm::vec3 playerRot = playerTransform.GetRotation();  // Get rotation from TransformComponent

			g_Audio.SetListenerPosition(playerPos, playerRot);
		}

		// Update the positions of all 3D sounds (including the fireplace)
		g_Audio.Update3DSoundPositions();

		g_ChangeText.startingRoomOnly = true;

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
			g_UI.Sniff(particleEntities ,goalnode, static_cast<float>(deltaTime));
			g_DialogueText.OnUpdate(deltaTime);

			if (!g_DialogueText.dialogueActive)
			{
				// let the change text finish first then allow pauseLogic
				if (!initChecklist)
				{
					g_Checklist.OnInitialize();
					g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do1");
					g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.15f, 0.05f), "Do2");
					g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.15f, 0.05f), "Do3");
					g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.15f, 0.05f), "Do4");
					initChecklist = true;
				}
			}

			if (!g_Checklist.shutted)
			{
				g_Checklist.OnUpdate(deltaTime);

				if (!g_Checklist.Check3 && !g_RopeBreaker.playedRopeSnap1) 
				{
					g_RopeBreaker.OnUpdate(deltaTime);
				}

				if (g_Checklist.Check3 && g_RopeBreaker.playedRopeSnap1 && !destroyedRope)
				{
					g_Coordinator.DestroyEntity(RopeObj);
					destroyedRope = true;
				}
			}

			// Take this away once u shift to script
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


			if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 0)
			{
				bark = false;
			}
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

			// Reset sniff state when the key is released
			if (g_Input.GetKeyState(GLFW_KEY_E) == 0)
			{
				sniff = false;
			}

			// until here

			if (g_Checklist.shutted && !g_DialogueText.dialogueActive && transitionActive == false)
			{
				if (g_Coordinator.GetComponent<CollisionComponent>(playerEnt).GetLastCollidedObjectName() == "WallHole")
				{
					transitionActive = true;
					transitionTimer = 0.0f;
				}
			}

			// If transition is active, update timer and render the effect.
			if (transitionActive)
			{
				transitionTimer += static_cast<float>(deltaTime);
				float progress = transitionTimer / transitionDuration; // Progress from 0.0 to 1.0.
				// Call the GraphicsSystem's modern transition effect.
				g_Coordinator.GetSystem<GraphicsSystem>()->RenderTransitionEffect(progress);

				if (transitionTimer >= transitionDuration)
				{
					// Transition complete, trigger level change.
					auto* loading = dynamic_cast<LoadingLevel*>(g_LevelManager.GetLevel("LoadingLevel"));
					if (loading)
					{
						loading->m_NextScene = "TimeRush";
						g_LevelManager.SetNextLevel("LoadingLevel");
					}
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
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/ambienceSFX.wav");

		if (g_Coordinator.HaveComponent<AudioComponent>(FireSound)) {
			auto& music = g_Coordinator.GetComponent<AudioComponent>(FireSound);
			music.StopAudio();
		}

		g_UI.inStartingRoom = false;
		g_ChangeText.startingRoomOnly = false;
		g_Checklist.Check3 = false;
		bark = false;
		sniff = false;
		initChecklist = false;
		destroyedRope = false;
		transitionActive = false;
		transitionTimer = 0.0f;
		g_Audio.Stop(BedRoomBGM);

		g_Coordinator.GetSystem<MyPhysicsSystem>()->ClearAllBodies();
		g_Coordinator.ResetEntities();
	}

private:
	bool camerachange = false;

	void ResetLevelState()
	{
		bark = false;
		sniff = false;
		initChecklist = false;
		savedcamdir = false;
		transitionActive = false;
		transitionTimer = 0.0f;
		camdir = glm::vec3(0.0f);
		particleEntities.clear();
		sniffCooldownTimer = 0.0;
		isSniffOnCooldown = false;
		camerachange = false;
		destroyedRope = false;
		// If needed, reset any other member variables here.
	}
};