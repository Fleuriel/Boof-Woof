#include "pch.h"
#include "UI.h"
#include "ResourceManager/ResourceManager.h"
#include "../Core/AssetManager/FilePaths.h"

UI g_UI;
Serialization UIstuff;

bool UI::savedE = true;
float UI::savedParticleTimer = 0.0f;
float UI::savedSniffAnimationTimer = 0.0f;
int UI::savedCurrCol = 0;

int UI::savedStaminaIndex = 4;  // Default full stamina
float UI::savedSprintTimer = 0.0f;
float UI::savedReplenishTimer = 0.0f;
float UI::savedPelletOpacities[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }; // Default: Full opacity

bool UI::isExhausted = false;

void UI::OnInitialize()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/UIcon.json");
	storage = UIstuff.GetStored();

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
	std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
	{
		{"CDSniff", [&](Entity entity) { CDSniff = entity; }},
		{"Sniff", [&](Entity entity) { Sniffa = entity; }},
		{"Sprint", [&](Entity entity) { Sprinto = entity; }},
		{"Pellet1", [&](Entity entity) { P1 = entity; }},
		{"Pellet2", [&](Entity entity) { P2 = entity; }},
		{"Pellet3", [&](Entity entity) { P3 = entity; } },
		{"Pellet4", [&](Entity entity) { P4 = entity; }},
		{"Pellet5", [&](Entity entity) { P5 = entity; }}
	};

	for (auto entity : entities)
	{
		const auto& metadata = g_Coordinator.GetComponent<MetadataComponent>(entity);
		auto it = nameToAction.find(metadata.GetName());

		if (it != nameToAction.end())
		{
			it->second(entity);
		}
	}

	// Restore the CD and everything
	canPressE = savedE;
	particleTimer = savedParticleTimer;
	sniffAnimationTimer = savedSniffAnimationTimer;

	staminaIndex = savedStaminaIndex;
	sprintTimer = savedSprintTimer;
	replenishTimer = savedReplenishTimer;

	if (g_Coordinator.HaveComponent<UIComponent>(CDSniff))
	{
		UIComponent& sniffa = g_Coordinator.GetComponent<UIComponent>(CDSniff);
		sniffa.set_curr_col(savedCurrCol);
		sniffa.set_playing(!canPressE);
	}

	// Restore Pellets' Opacity Based on Stamina
	pellets = { P5, P4, P3, P2, P1 };
	for (int i = 0; i < 5; i++)
	{
		if (g_Coordinator.HaveComponent<UIComponent>(pellets[i]))
		{
			UIComponent& pellet = g_Coordinator.GetComponent<UIComponent>(pellets[i]);
			pellet.set_opacity(savedPelletOpacities[i]);
		}
	}
}

void UI::OnUpdate(double deltaTime)
{
	Sprint(static_cast<float>(deltaTime));
}

void UI::OnShutdown()
{
	// Save the CD and everything
	savedE = canPressE;
	savedParticleTimer = particleTimer;
	savedSniffAnimationTimer = sniffAnimationTimer;

	// Save Sprint Stamina State
	savedStaminaIndex = staminaIndex;
	savedSprintTimer = sprintTimer;
	savedReplenishTimer = replenishTimer;

	for (int i = 0; i < 5; i++)
	{
		if (g_Coordinator.HaveComponent<UIComponent>(pellets[i]))
		{
			UIComponent& pellet = g_Coordinator.GetComponent<UIComponent>(pellets[i]);
			savedPelletOpacities[i] = pellet.get_opacity(); // Store opacity
		}
	}

	if (g_Coordinator.HaveComponent<UIComponent>(CDSniff))
	{
		UIComponent& sniffa = g_Coordinator.GetComponent<UIComponent>(CDSniff);
		savedCurrCol = sniffa.get_curr_col();
	}

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto i = entities.begin(); i != entities.end(); i++)
	{
		for (auto k = storage.begin(); k != storage.end(); k++)
		{
			if (*k == *i)
			{
				g_Coordinator.DestroyEntity(*i);
			}
		}
	}
}

void UI::Sniff(const std::vector<Entity>& particles, float dt)
{
	if (!g_Coordinator.HaveComponent<UIComponent>(CDSniff))
		return;

	UIComponent& sniffa = g_Coordinator.GetComponent<UIComponent>(CDSniff);
	UIComponent& sniffaAnimation = g_Coordinator.GetComponent<UIComponent>(Sniffa);

	if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && canPressE) 
	{
		sniffa.set_frame_interval(1.0f);
		sniffa.set_playing(true);
		sniffaAnimation.set_playing(true);
		canPressE = false;

		sniffAnimationTimer = 5.0f;

		// Activate particles for all entities
		for (Entity particleEntity : particles)
		{
			if (g_Coordinator.HaveComponent<ParticleComponent>(particleEntity))
			{
				ParticleComponent& particle = g_Coordinator.GetComponent<ParticleComponent>(particleEntity);
				particle.setOpacity(1.0f);
			}
		}

		particleTimer = 3.0f; // 3 seconds
	}

	if (particleTimer > 0.0f) 
	{
		particleTimer -= dt;
		if (particleTimer <= 0.0f) 
		{
			for (Entity particleEntity : particles)
			{
				if (g_Coordinator.HaveComponent<ParticleComponent>(particleEntity))
				{
					ParticleComponent& particle = g_Coordinator.GetComponent<ParticleComponent>(particleEntity);
					particle.setOpacity(0.0f);
					sniffaAnimation.set_playing(false);
				}
			}
		}
	}

	if (sniffa.get_playing()) 
	{
		if (sniffAnimationTimer > 0.0f)
		{
			sniffAnimationTimer -= dt;  // Countdown delay before checking frame 0
		}
		else 
		{
			if (sniffa.get_curr_col() == 0)
			{
				sniffAnimationTimer = 0.0f;
				sniffa.set_playing(false);
				canPressE = true;
			}
		}
	}
}

void UI::Sprint(float dt)
{
	if (!g_Coordinator.HaveComponent<UIComponent>(Sprinto))
		return;

	UIComponent& sprinto = g_Coordinator.GetComponent<UIComponent>(Sprinto);

	static bool isSprinting = false; // Track sprint state

	// Ensure we have 5 pellets
	if (pellets.size() < 5)
		return;

	// Check if Shift is held for sprinting
	if (g_Input.GetKeyState(GLFW_KEY_LEFT_SHIFT) >= 1 && !isExhausted)
	{
		sprinto.set_playing(true);
		isSprinting = true;
		replenishTimer = 0.0f; // Reset replenish timer while sprinting

		sprintTimer += dt; // Increase sprint timer

		// If holding Shift for 1.5s, deplete one pellet from 4 3 2 1 0
		if (sprintTimer >= 1.5f && staminaIndex >= 0)
		{
			if (g_Coordinator.HaveComponent<UIComponent>(pellets[staminaIndex]))
			{
				UIComponent& pellet = g_Coordinator.GetComponent<UIComponent>(pellets[staminaIndex]);
				pellet.set_opacity(0.0f); // Deplete pellet
			}
			staminaIndex--; // Move to next pellet
			staminaIndex = std::max(staminaIndex, -1); // Ensure it never goes below -1

			sprintTimer = 0.0f; // Reset sprint timer
		}

		if (staminaIndex == -1)
		{
			isExhausted = true; // Prevent further sprinting
		}
	}
	else
	{
		// stop sprint animation when shift released or when stamina gone
		sprinto.set_playing(false);

		// Sregen as long as not full pellets
		if (staminaIndex < 4) 
		{
			replenishTimer += dt; // Increase replenish timer

			// Every 3 seconds, restore one pellet
			if (replenishTimer >= 3.0f)
			{
				staminaIndex++; // Move back one pellet
				staminaIndex = std::min(staminaIndex, 4); // Ensure it never goes above 4

				if (g_Coordinator.HaveComponent<UIComponent>(pellets[staminaIndex]))
				{
					UIComponent& pellet = g_Coordinator.GetComponent<UIComponent>(pellets[staminaIndex]);
					pellet.set_opacity(1.0f); // Restore pellet
				}

				replenishTimer = 0.0f; // Reset replenish timer
			}

			// Allow sprinting again if at least one pellet is restored
			if (staminaIndex >= 0)
			{
				isExhausted = false; 
			}
		}
	}
}
