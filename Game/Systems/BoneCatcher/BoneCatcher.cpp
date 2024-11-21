#include "BoneCatcher.h"

BoneCatcher g_BoneCatcher;
Serialization serial;

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
					CatchZonePos = g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).GetPosition();
					CatchZoneScale = g_Coordinator.GetComponent<TransformComponent>(m_CatchZone).GetScale();

					BoxMin = CatchZonePos - CatchZoneScale * 0.5f;	// Bottom left
					BoxMax = CatchZonePos + CatchZoneScale * 0.5f;  // Top right

					TeethPos = { DogPos.x + 0.05f, DogPos.y - (DogScale.y / 2) + (TeethScale.y / 2), 0.f };
				}
			}
		}
	}
}

void BoneCatcher::OnUpdate(double deltaTime)
{
	if (m_IsMoving)
	{
		MoveLeftRightVisual(deltaTime);
	}
	else 
	{
		BiteDownVisual(deltaTime);
	}

	bool isInRange = (TeethPos.x >= BoxMin.x && TeethPos.x <= BoxMax.x) && (TeethPos.y >= BoxMin.y && TeethPos.y <= BoxMax.y);

	if (isInRange)
	{
		std::cout << "Hit detected!" << std::endl;
		// Play YAY sound
		// pass, change catchzone to smaller & speed faster.
	}
	else
	{
		std::cout << "No hit." << std::endl;
		// Play BOO sound
		// Fail, same speed, same catchzone.
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
	

	//m_DestroyTimer = 2.0f;
	//m_ShouldDestroy = true;
}

void BoneCatcher::MoveLeftRightVisual(double deltaTime)
{
	// Update position based on direction and speed
	DogPos.x += m_Direction * m_Speed * static_cast<float>(deltaTime);
	TeethPos = { DogPos.x + 0.05f, DogPos.y - (DogScale.y / 2) + (TeethScale.y / 2), 0.f };

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
		DogPos.y -= 0.2f;

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
			DogPos.y += 0.2f;

			if (g_Coordinator.HaveComponent<TransformComponent>(m_DogHead))
			{
				g_Coordinator.GetComponent<TransformComponent>(m_DogHead).SetPosition(DogPos);
			}

			m_Down = false;
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