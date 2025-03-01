#include "UI.h"
#include "ResourceManager/ResourceManager.h"
#include "../Core/AssetManager/FilePaths.h"

UI g_UI;
Serialization UIstuff;

bool UI::savedE = true;
float UI::savedParticleTimer = 0.0f;
float UI::savedSniffAnimationTimer = 0.0f;
int UI::savedCurrCol = 0;

void UI::OnInitialize()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/UIcon.json");
	storage = UIstuff.GetStored();

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();
	std::unordered_map<std::string, std::function<void(Entity)>> nameToAction =
	{
		{"CDSniff", [&](Entity entity) { CDSniff = entity; }},
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

	if (g_Coordinator.HaveComponent<UIComponent>(CDSniff))
	{
		UIComponent& sniffa = g_Coordinator.GetComponent<UIComponent>(CDSniff);
		sniffa.set_curr_col(savedCurrCol);
		sniffa.set_playing(!canPressE);
	}
}

void UI::OnUpdate(double deltaTime)
{
	/* Empty by design */
}

void UI::OnShutdown()
{
	// Save the CD and everything
	savedE = canPressE;
	savedParticleTimer = particleTimer;
	savedSniffAnimationTimer = sniffAnimationTimer;

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

	if (g_Input.GetKeyState(GLFW_KEY_E) >= 1 && canPressE) 
	{
		sniffa.set_frame_interval(1.0f);
		sniffa.set_playing(true);
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
