#include "BoneCatcher.h"

BoneCatcher g_BoneCatcher;
Serialization serial;
std::random_device rd;
std::mt19937 gen{ rd() };  // Mersenne Twister random number generator
std::uniform_real_distribution<float> dist;  // Default distribution range

void BoneCatcher::OnLoad()
{
	g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/BoneCatcher.json");
	storage = serial.GetStored();
}

void BoneCatcher::OnInitialize()
{
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

					// Randomize the value, set it
					dist = std::uniform_real_distribution<float>(MinMaxPos.x, MinMaxPos.y);
					CatchZonePos.x = dist(gen);
					g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetPosition(CatchZonePos);

					// Get initial scale set from level editor first
					CatchZoneScale = g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).GetScale();

					// Randomize the value, set it
					dist = std::uniform_real_distribution<float>(MinMaxScale.x, MinMaxScale.y);
					CatchZoneScale.x = dist(gen);
					g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetScale(CatchZoneScale);

					BoxMin = CatchZonePos - CatchZoneScale * 0.5f;	// Bottom left
					BoxMax = CatchZonePos + CatchZoneScale * 0.5f;  // Top right

					TeethPos = { DogPos.x - 0.01f, DogPos.y - (DogScale.y / 2) + (TeethScale.y / 2), 0.f };
				}
			}
		}
	}
}

void BoneCatcher::OnUpdate(double deltaTime)
{
	if (g_Input.GetKeyState(GLFW_KEY_C) >= 1)
	{
		g_BoneCatcher.Stop();
	}

	if (m_IsMoving)
	{
		MoveLeftRightVisual(deltaTime);
	}
	else 
	{
		BiteDownVisual(deltaTime);		
	}

	

	if (m_ShouldDestroy)
	{
		m_DestroyTimer -= static_cast<float>(deltaTime); // decreasing timer based on deltatime
		std::cout << m_DestroyTimer << std::endl;
		if (m_DestroyTimer <= 0.0f)
		{
			ClearBoneCatcher();
			m_ShouldDestroy = false; // reset
		}
	}
}

void BoneCatcher::Stop()
{
	// Stop then visual feedback up down
	m_IsMoving = false;
	
	TeethPos = { DogPos.x - 0.01f, DogPos.y - (DogScale.y / 2) + (TeethScale.y / 2), 0.f };

	BoxMin = CatchZonePos - CatchZoneScale * 0.5f;	// Bottom left
	BoxMax = CatchZonePos + CatchZoneScale * 0.5f;  // Top right

	bool isInRange = (TeethPos.x >= BoxMin.x && TeethPos.x <= BoxMax.x) && (TeethPos.y >= BoxMin.y && TeethPos.y <= BoxMax.y);

	if (isInRange && !m_HitDetected)
	{
		// std::cout << "Hit detected!" << std::endl;
		m_HitDetected = true;

		// Play YAY sound
		// pass, change catchzone to smaller & speed faster.

		// Randomize X position (must stay within the smaller range after each hit)
		dist = std::uniform_real_distribution<float>(MinMaxPos.x, MinMaxPos.y);
		CatchZonePos.x = dist(gen);
		g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetPosition(CatchZonePos);

		// Randomize the X scale (must stay within the smaller range after each hit)
		dist = std::uniform_real_distribution<float>(MinMaxScale.x, CatchZoneScale.x);
		CatchZoneScale.x = dist(gen);
		g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).SetScale(CatchZoneScale);
	}
	else
	{
		//std::cout << "No hit." << std::endl;
		// Play BOO sound
		// Fail, same speed, same catchzone.
	}

	// Comment out first
	//m_DestroyTimer = 2.0f;
	//m_ShouldDestroy = true;
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

void BoneCatcher::BiteDownVisual(double deltaTime)
{
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

			m_Down = false;
			m_HitDetected = false;
			m_IsMoving = true;
		}
	}
}
void BoneCatcher::ClearBoneCatcher()
{
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