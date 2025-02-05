#include "pch.h"
#include "TimerTR.h"
#include <AssetManager/FilePaths.h>

TimerTR g_TimerTR;
Serialization serialTimer;

void TimerTR::OnInitialize()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/Timer.json");
	storage = serialTimer.GetStored();
		
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Timer")
			{
				m_Timer = entity;
				break;
			}
		}
	}
}

void TimerTR::OnUpdate(double deltaTime)
{
	if (g_Coordinator.HaveComponent<FontComponent>(m_Timer)) 
	{
		auto& text = g_Coordinator.GetComponent<FontComponent>(m_Timer);

		// Decrement timer by deltaTime
		timer -= deltaTime;

		// If the timer has gone past 0, set it to 0 (so it doesn't go negative)
		if (timer < 0.0)
		{
			timer = 0.0;
			text.set_pos(glm::vec2(0.748f, 0.76f));
			text.set_scale(glm::vec2(0.8f, 2.1f));
			text.set_text("Times Up!");
			text.set_color(glm::vec3(255,0,0));
		}

		if (timer != 0.0) 
		{
			// Format the time in minutes and seconds
			int minutes = static_cast<int>(timer) / 60;
			int seconds = static_cast<int>(timer) % 60;

			// Display the formatted time as "MM:SS"
			text.set_text(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds));
		}
	}
	else
	{
		timer = 180.0;
	}
}

void TimerTR::OnShutdown()
{
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

	timer = 180.0;
}

void TimerTR::Reset()
{
	timer = 180.0;
}
