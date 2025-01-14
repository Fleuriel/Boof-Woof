#include "BoneCatcher.h"
#include "../RopeBreaker/RopeBreaker.h"
#include "../Core/AssetManager/FilePaths.h"

BoneCatcher g_BoneCatcher;
Serialization serial;
std::random_device rd;
std::mt19937 gen{ rd() };  // Mersenne Twister random number generator
std::uniform_real_distribution<float> dist;  // Default distribution range

void BoneCatcher::OnInitialize()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/BoneCatcher.json");

	storage = serial.GetStored();

	// Retrieve the head
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Head")
			{
				m_DogHead = entity;

				if (g_Coordinator.HaveComponent<TransformComponent>(m_DogHead))
				{
					DogPos = g_Coordinator.GetComponent<TransformComponent>(m_DogHead).GetPosition();
					DogScale = g_Coordinator.GetComponent<TransformComponent>(m_DogHead).GetScale();
					initialDogPos = DogPos;
				}
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "CatchZone")
			{
				m_CatchZone = entity;

				if (g_Coordinator.HaveComponent<TransformComponent>(m_CatchZone))
				{
					// Get initial position set from level editor first
					CatchZonePos = g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).GetPosition();

					// Randomize the position, set it
					dist = std::uniform_real_distribution<float>(MinMaxPos.x, MinMaxPos.y);
					CatchZonePos.x = dist(gen);
					g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetPosition(CatchZonePos);

					// Get initial scale set from level editor first
					CatchZoneScale = g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).GetScale();

					// Randomize the scale, set it
					//dist = std::uniform_real_distribution<float>(MinMaxScale.x, MinMaxScale.y);
					//CatchZoneScale.x = dist(gen);
					//g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetScale(CatchZoneScale);

					BoxMin = CatchZonePos - CatchZoneScale * 0.5f;	// Bottom left
					BoxMax = CatchZonePos + CatchZoneScale * 0.5f;  // Top right

					TeethPos = { DogPos.x - 0.01f, DogPos.y - (DogScale.y / 2) + (TeethScale.y / 2), 0.f };
				}
			}
		}
	}

	m_Speed = 0.5f;
	m_HitCount = 0;
}

void BoneCatcher::OnUpdate(double deltaTime)
{
	ClearBoneCatcherTimer += deltaTime;

	if (m_HitCount <= 4)
	{
		if (g_Input.GetKeyState(GLFW_KEY_C) >= 1)
		{
			// Stop then visual feedback up down
			m_IsMoving = false;
		}

		if (m_IsMoving)
		{
			MoveLeftRightVisual(deltaTime);
		}
		else
		{
			BiteDown(deltaTime);
		}
	}

	// Play for as long bonecatcher lasts.
	if (AudioTimer <= ClearBoneCatcherTimer && !isAudioPlaying)
	{
		g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/CreakingRope2.wav", true, "SFX");
		isAudioPlaying = true;
	}

	Stop(deltaTime);
}

void BoneCatcher::Stop(double deltaTime)
{

	// After 5 times, no more
	if (m_HitCount == 5 && !m_ShouldDestroy)
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/CreakingRope2.wav");
		m_DestroyTimer = 2.0f;
		m_ShouldDestroy = true;
	}

	if (m_ShouldDestroy)
	{
		m_DestroyTimer -= static_cast<float>(deltaTime); // decreasing timer based on deltatime
		// std::cout << m_DestroyTimer << std::endl;
		if (m_DestroyTimer <= 0.0f)
		{
			ClearBoneCatcher();
			m_ShouldDestroy = false; // reset
			m_HitCount = 0;

			if (g_RopeBreaker.RopeCount != 0)
			{
				g_RopeBreaker.RopeCount -= 1;

				// Despawn the rope
				g_RopeBreaker.DespawnRope();
			}
		}
	}
}

void BoneCatcher::MoveLeftRightVisual(double deltaTime)
{
	// Update position based on direction and speed
	DogPos.x += m_Direction * m_Speed * static_cast<float>(deltaTime);

	if (g_Coordinator.HaveComponent<TransformComponent>(m_DogHead))
	{
		g_Coordinator.GetComponent<TransformComponent>(m_DogHead).SetPosition(DogPos);
	}

	// Check boundaries and reverse direction if necessary
	if (DogPos.x >= m_MaxPos)
	{
		DogPos.x = m_MaxPos;
		m_Direction = -1; // Move left
	}
	if (DogPos.x <= m_MinPos)
	{
		DogPos.x = m_MinPos;
		m_Direction = 1; // Move right
	}
}

// Check whether collided with catchzone + visual feedback
void BoneCatcher::BiteDown(double deltaTime)
{
	// Catchzone Position changes each time so need to check here to update
	BoxMin = CatchZonePos - CatchZoneScale * 0.5f;	// Bottom left
	BoxMax = CatchZonePos + CatchZoneScale * 0.5f;  // Top right

	TeethPos = { DogPos.x - 0.01f, DogPos.y - (DogScale.y / 2) + (TeethScale.y / 2), 0.f };
	bool isInRange = (TeethPos.x >= BoxMin.x && TeethPos.x <= BoxMax.x) && (TeethPos.y >= BoxMin.y && TeethPos.y <= BoxMax.y);

	if (!m_Down)
	{
		DogPos.y -= 0.02f;

		if (g_Coordinator.HaveComponent<TransformComponent>(m_DogHead))
		{
			g_Coordinator.GetComponent<TransformComponent>(m_DogHead).SetPosition(DogPos);
		}

		m_Down = true;  // prevent moving down again until reset.

		// 2 seconds to wait before moving back up
		m_DownTimer = 2.0f;

		// Check for collision 
		if (isInRange && !m_HitDetected)
		{
			//std::cout << "Hit detected!" << std::endl;
			m_HitDetected = true;
			m_HitCount += 1;
			//std::cout << "m_HitCount: " << m_HitCount  << std::endl;

			// Play YAY sound
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/CorrectSound.wav", false, "SFX");

			// Hit = Pass = Randomize Catchzone position & Faster DogHead Speed.
			m_Speed += 0.2f;

			// Randomize X position (must stay within the smaller range after each hit)
			dist = std::uniform_real_distribution<float>(MinMaxPos.x, MinMaxPos.y);
			CatchZonePos.x = dist(gen);

			// Randomize the X scale (must stay within the smaller range after each hit) - if want to change scale.
			//dist = std::uniform_real_distribution<float>(MinMaxScale.x, CatchZoneScale.x);
			//CatchZoneScale.x = dist(gen);
		}
		else {
			// Play BOO sound
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/WrongSound.wav", false, "SFX");
			// Failed to hit - nothing changes, play the same level.
		}
	}

	// Decrease the timer and move head back up after 2 seconds
	if (m_Down && m_DownTimer > 0.0f)
	{
		m_DownTimer -= static_cast<float>(deltaTime);

		if (m_DownTimer <= 0.0f)
		{
			// Move the dog head back up
			DogPos.y += 0.02f;

			if (g_Coordinator.HaveComponent<TransformComponent>(m_DogHead))
			{
				g_Coordinator.GetComponent<TransformComponent>(m_DogHead).SetPosition(DogPos);
			}

			if (m_HitDetected)
			{
				if (g_Coordinator.HaveComponent<TransformComponent>(m_CatchZone))
				{
					g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetPosition(CatchZonePos);
					//g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetScale(CatchZoneScale);
				}
			}

			m_Down = false;
			m_HitDetected = false;
			m_IsMoving = true;
		}
	}
}
void BoneCatcher::ClearBoneCatcher()
{
	AudioTimer = ClearBoneCatcherTimer;


	// Stop the audio when bonecatcher is cleared
	if (isAudioPlaying)
	{
		std::cout << "Entered audioplaying" << std::endl;
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO+"/CreakingRope2.wav"); // Stop the specific file path
		isAudioPlaying = false;  // Reset the flag
	}

	// Just remove whatever we had stored from the current alive entity and destroy them
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