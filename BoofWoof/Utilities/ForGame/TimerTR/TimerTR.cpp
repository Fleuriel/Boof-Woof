#include "pch.h"
#include "TimerTR.h"
#include <AssetManager/FilePaths.h>



TimerTR g_TimerTR;
Serialization serialTimer;

static glm::vec2 startScale{};
static glm::vec2 endScale{};


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
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Minus10")
			{
				m_Minus = entity;
				break;
			}
		}
	}
}

static double tenSecondsTimer = 10.0;
static double scaleTransitionTimer = 0.0;
const double scaleDuration = 1.0; // 1 second duration
static bool isScalingDown = false;
static int lastScaledSecond = -1;  // Prevents multiple triggers in the same second

void TimerTR::OnUpdate(double deltaTime)
{
	if (!g_Coordinator.HaveComponent<FontComponent>(m_Minus))
		return;

	auto& minus = g_Coordinator.GetComponent<FontComponent>(m_Minus);

	if (g_Coordinator.HaveComponent<FontComponent>(m_Timer)) 
	{
		auto& text = g_Coordinator.GetComponent<FontComponent>(m_Timer);

		// Decrement timer by deltaTime
		timer -= deltaTime;
		tenSecondsTimer -= deltaTime;

		// If the timer has gone past 0, set it to 0 (so it doesn't go negative)
		if (timer < 0.0)
		{
			timer = 0.0;
			text.set_pos(glm::vec2(0.748f, 0.76f));
			text.set_scale(glm::vec2(0.8f, 2.1f));
			text.set_text("Times Up!");
			text.set_color(glm::vec3(255,0,0));
		}

		if (startScale == glm::vec2()) {
			startScale = text.get_scale() * 1.2f; // Enlarged scale
		}

		if (endScale == glm::vec2()) {
			endScale = text.get_scale();	// Normal scale
		}

		if (timer != 0.0)
		{
			int minutes = static_cast<int>(timer) / 60;
			int seconds = static_cast<int>(timer) % 60;

			if (timer > 10.0 && !isScalingDown)
				text.set_color(glm::vec3(1.0f, 1.0f, 1.0f));

			if (tenSecondsTimer <= 0.0)
			{
				// Instantly set to larger scale
				text.set_scale(startScale);
				text.set_color(glm::vec3(1.0f, 0.0f, 0.0f));

				// Start shrinking back
				scaleTransitionTimer = 0.0;
				isScalingDown = true;

				// Reset 10-second timer
				tenSecondsTimer = 10.0;
			}

			// **Trigger scale-up every second when timer < 10**
			if (timer <= 10.0 && seconds != lastScaledSecond)
			{
				if (text.get_color() != glm::vec3(1, 0, 0))
					text.set_color(glm::vec3(1, 0, 0));
				text.set_scale(startScale);
				scaleTransitionTimer = 0.0;
				isScalingDown = true;
				lastScaledSecond = seconds;  // Prevent duplicate triggers in the same second
			}

			if (isScalingDown)
			{
				// Scale back down over 1 second
				scaleTransitionTimer += deltaTime;
				float factor = std::clamp(static_cast<float>(scaleTransitionTimer / scaleDuration), 0.0f, 1.0f);
				glm::vec2 newScale = glm::mix(startScale, endScale, factor);
				text.set_scale(newScale);

				// Stop shrinking after 1 second
				if (scaleTransitionTimer >= scaleDuration)
				{
					isScalingDown = false;
				}
			}

			text.set_text(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds));
		}
	}
	else
	{
		timer = 30.0;
	}

	if (touched) 
	{
		// Show the world
		if (!scaleUp && timer != 0.0)
		{
			minus.set_scale(glm::vec2(0.6f, 1.0f));
			scaleUp = true;
		}
	}
	else 
	{
		minus.set_scale(glm::vec2(0.0f, 0.0f));
		scaleUp = false;
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

	storage.clear();
	timer = 30.0;
}

void TimerTR::Reset()
{
	timer = 30.0;
	touched = false;
	scaleUp = false;
}
