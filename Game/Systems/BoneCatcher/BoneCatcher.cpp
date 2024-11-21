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
					initialDogPos = g_Coordinator.GetComponent<TransformComponent>(m_DogHead).GetPosition();
				}

				break;
			}
		}
	}
}

void BoneCatcher::OnUpdate(double deltaTime)
{
	if (m_IsMoving)
	{
		// Update position based on direction and speed
		initialDogPos.x += m_Direction * m_Speed * static_cast<float>(deltaTime);

		if (g_Coordinator.HaveComponent<TransformComponent>(m_DogHead))
		{
			g_Coordinator.GetComponent<TransformComponent>(m_DogHead).SetPosition(initialDogPos);
		}

		// Check boundaries and reverse direction if necessary
		if (initialDogPos.x >= m_MaxPos)
		{
			initialDogPos.x = m_MaxPos;
			m_Direction = -1; // Move left
		}
		if (initialDogPos.x <= m_MinPos)
		{
			initialDogPos.x = m_MinPos;
			m_Direction = 1; // Move right
		}
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
	m_IsMoving = false;
	m_DestroyTimer = 2.0f;
	m_ShouldDestroy = true;
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
