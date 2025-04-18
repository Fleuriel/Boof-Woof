#include "BoneCatcher.h"
#include "../RopeBreaker/RopeBreaker.h"
#include "../CageBreaker/CageBreaker.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/UI/UI.h"
#include "../ChangeText/ChangeText.h"
#include "../Checklist/Checklist.h"

BoneCatcher g_BoneCatcher;
Serialization serial;
std::random_device rd;
std::mt19937 gen{ rd() };  // Mersenne Twister random number generator
std::uniform_real_distribution<float> dist;  // Default distribution range

void BoneCatcher::OnInitialize()
{
	if (!UIClosed)
	{
		g_UI.OnShutdown();

		if (isCage)
		{
			g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/CageBreaker.json");
		}

		if (isRope)
		{
			g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/RopeBreaker.json");
		}

		UIClosed = true;
	}	

	storage = serial.GetStored();

	// Retrieve the head
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Base")
			{
				m_Base = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Head")
			{
				m_DogHead = entity;

				if (g_Coordinator.HaveComponent<UIComponent>(m_DogHead))
				{
					DogPos = g_Coordinator.GetComponent<UIComponent>(m_DogHead).get_position();
				}
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "CatchZone")
			{
				m_CatchZone = entity;

				if (g_Coordinator.HaveComponent<UIComponent>(m_CatchZone))
				{
					// Get initial position set from level editor first
					CatchZonePos = g_Coordinator.GetComponent<UIComponent>(m_CatchZone).get_position();

					// Randomize the position, set it
					dist = std::uniform_real_distribution<float>(MinMaxPos.x, MinMaxPos.y);
					CatchZonePos.x = dist(gen);
					g_Coordinator.GetComponent<UIComponent>(m_CatchZone).set_position(CatchZonePos);

					// Get initial scale set from level editor first
					CatchZoneScale = g_Coordinator.GetComponent<UIComponent>(m_CatchZone).get_scale();

					BoxMin = CatchZonePos - CatchZoneScale * 0.5f;	// Bottom left
					BoxMax = CatchZonePos + CatchZoneScale * 0.5f;  // Top right

					TeethPos = { DogPos.x - 0.02f, DogPos.y - 0.27 };
				}
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Puppy1")
			{
				m_Puppy1 = entity;

				if (g_Coordinator.HaveComponent<UIComponent>(m_Puppy1))
				{
					Puppy1Pos = g_Coordinator.GetComponent<UIComponent>(m_Puppy1).get_position();
				}
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Puppy2")
			{
				m_Puppy2 = entity;

				if (g_Coordinator.HaveComponent<UIComponent>(m_Puppy2))
				{
					Puppy2Pos = g_Coordinator.GetComponent<UIComponent>(m_Puppy2).get_position();
				}
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Puppy3")
			{
				m_Puppy3 = entity;

				if (g_Coordinator.HaveComponent<UIComponent>(m_Puppy3))
				{
					Puppy3Pos = g_Coordinator.GetComponent<UIComponent>(m_Puppy3).get_position();
				}
			}
		}
	}

	isActive = true;
	m_BaseChanged = false;
	m_Speed = 0.5f;
	m_HitCount = 0;
	savePawgress = false;

	if (isCage)
	{
		std::vector<Entity> checklistEnt = { g_Checklist.Paper, g_Checklist.Do1, g_Checklist.Do2, g_Checklist.Do3, g_Checklist.Box1, g_Checklist.Box2, g_Checklist.Box3 };
		g_Checklist.HideChecklistUI(checklistEnt, true);

		if (g_CageBreaker.CageHitCounts.count(1) > 0 && g_CageBreaker.Cage1Colliding)
		{
			m_HitCount = g_CageBreaker.CageHitCounts[1];
			m_Speed = g_CageBreaker.speedCage[1];
			m_Direction = g_CageBreaker.directionCage[1];

			if (g_Coordinator.GetComponent<GraphicsComponent>(g_CageBreaker.cage1).getModelName() == "Cage")
			{
				if (g_Coordinator.HaveComponent<AnimationComponent>(g_CageBreaker.cage1))
				{
					g_Coordinator.GetComponent<AnimationComponent>(g_CageBreaker.cage1).pauseAnimation = false;
				}
			}
		}
		else if (g_CageBreaker.CageHitCounts.count(2) > 0 && g_CageBreaker.Cage2Colliding)
		{
			m_HitCount = g_CageBreaker.CageHitCounts[2];
			m_Speed = g_CageBreaker.speedCage[2];
			m_Direction = g_CageBreaker.directionCage[2];

			if (g_Coordinator.GetComponent<GraphicsComponent>(g_CageBreaker.cage2).getModelName() == "Cage1")
			{
				if (g_Coordinator.HaveComponent<AnimationComponent>(g_CageBreaker.cage2))
				{
					g_Coordinator.GetComponent<AnimationComponent>(g_CageBreaker.cage2).pauseAnimation = false;
				}
			}
		}
		else if (g_CageBreaker.CageHitCounts.count(3) > 0 && g_CageBreaker.Cage3Colliding)
		{
			m_HitCount = g_CageBreaker.CageHitCounts[3];
			m_Speed = g_CageBreaker.speedCage[3];
			m_Direction = g_CageBreaker.directionCage[3];

			if (g_Coordinator.GetComponent<GraphicsComponent>(g_CageBreaker.cage3).getModelName() == "Cage2")
			{
				if (g_Coordinator.HaveComponent<AnimationComponent>(g_CageBreaker.cage3))
				{
					g_Coordinator.GetComponent<AnimationComponent>(g_CageBreaker.cage3).pauseAnimation = false;
				}
			}
		}

		UpdatePuppyHeads();		
	}

	if (isRope)
	{
		if (g_ChangeText.startingRoomOnly) 
		{
			std::vector<Entity> checklistEnt = { g_Checklist.Paper, g_Checklist.Do1, g_Checklist.Do2, g_Checklist.Do3, g_Checklist.Do4, g_Checklist.Box1, g_Checklist.Box2, g_Checklist.Box3, g_Checklist.Box4 };
			g_Checklist.HideChecklistUI(checklistEnt, true);
		}

		if (g_UI.finishCaged)
		{
			std::vector<Entity> checklistEnt = { g_Checklist.Paper, g_Checklist.Do1, g_Checklist.Box1 };
			g_Checklist.HideChecklistUI(checklistEnt, true);
		}

		if (g_RopeBreaker.RopeHitCounts.count(1) > 0 && g_RopeBreaker.Rope1Colliding)
		{
			m_HitCount = g_RopeBreaker.RopeHitCounts[1];
			m_Speed = g_RopeBreaker.speedRope[1];
			m_Direction = g_RopeBreaker.directionRope[1];
		}
		else if (g_RopeBreaker.RopeHitCounts.count(2) > 0 && g_RopeBreaker.Rope2Colliding)
		{
			m_HitCount = g_RopeBreaker.RopeHitCounts[2];
			m_Speed = g_RopeBreaker.speedRope[2];
			m_Direction = g_RopeBreaker.directionRope[2];
		}

		UpdatePuppyHeads();
	}
}

void BoneCatcher::OnUpdate(Entity& entity, double deltaTime)
{

	

	ClearBoneCatcherTimer += deltaTime;

	if (m_HitCount <= (m_NoOfHitsRequired - 1))
	{
		if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 1)
		{
			// Stop then visual feedback up down
			m_IsMoving = false;
		}

		if (isRope)
		{
			ChangeBase("RopeSemi", "RopeBreak");
		}

		if (isCage)
		{
			ChangeBase("BarSemi", "BarBreak");
		}

		if (m_HitCount == (m_NoOfHitsRequired + 1) / 2)
		{
			m_BaseChanged = false;
		}

		if (m_IsMoving)
		{
			MoveLeftRightVisual(deltaTime);
		}
		else
		{
			BiteDown(entity, deltaTime);
		}
	}

	// Play for as long bonecatcher lasts.
	if (AudioTimer <= ClearBoneCatcherTimer && !isAudioPlaying)
	{
		if (isRope)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CreakingRope2.wav", true, "SFX");
		}
		else if (isCage)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/MetalCage.wav", true, "SFX");

		}
		isAudioPlaying = true;
	}

	Stop(deltaTime);
}

void BoneCatcher::Stop(double deltaTime)
{
	// After 5 times, no more
	if (m_HitCount == m_NoOfHitsRequired && !m_ShouldDestroy)
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/CreakingRope2.wav");
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

			if (isCage)
			{
				g_CageBreaker.DespawnCage();
			}

			if (isRope)
			{
				if (g_RopeBreaker.RopeCount != 0)
				{
					g_RopeBreaker.RopeCount -= 1;

					// Despawn the rope
					if (!g_ChangeText.startingRoomOnly) 
					{
						g_RopeBreaker.DespawnRope();
					}
					else 
					{
						g_RopeBreaker.DespawnRopeInStartingRoom();
					}
				}
			}
		}
	}
}

void BoneCatcher::MoveLeftRightVisual(double deltaTime)
{
	// Update position based on direction and speed
	DogPos.x += m_Direction * m_Speed * static_cast<float>(deltaTime);
	Puppy1Pos.x += m_Direction * m_Speed * static_cast<float>(deltaTime);
	Puppy2Pos.x += m_Direction * m_Speed * static_cast<float>(deltaTime);
	Puppy3Pos.x += m_Direction * m_Speed * static_cast<float>(deltaTime);


	if (g_Coordinator.HaveComponent<UIComponent>(m_DogHead))
	{
		g_Coordinator.GetComponent<UIComponent>(m_DogHead).set_position(DogPos);
		g_Coordinator.GetComponent<UIComponent>(m_Puppy1).set_position(Puppy1Pos);
		g_Coordinator.GetComponent<UIComponent>(m_Puppy2).set_position(Puppy2Pos);
		g_Coordinator.GetComponent<UIComponent>(m_Puppy3).set_position(Puppy3Pos);
	}

	// Check boundaries and reverse direction if necessary
	if (DogPos.x >= m_MaxPos)
	{
		DogPos.x = m_MaxPos;
		Puppy1Pos.x = m_SmollMaxPos;
		Puppy2Pos.x = m_SmollMaxPos;
		Puppy3Pos.x = m_SmollMaxPos;

		m_Direction = -1; // Move left
	}
	if (DogPos.x <= m_MinPos)
	{
		DogPos.x = m_MinPos;
		Puppy1Pos.x = m_SmollMinPos;
		Puppy2Pos.x = m_SmollMinPos;
		Puppy3Pos.x = m_SmollMinPos;

		m_Direction = 1; // Move right
	}
}

// Check whether collided with catchzone + visual feedback
void BoneCatcher::BiteDown(Entity &entity, double deltaTime)
{
	// Catchzone Position changes each time so need to check here to update
	BoxMin = CatchZonePos - CatchZoneScale * 0.5f;	// Bottom left
	BoxMax = CatchZonePos + CatchZoneScale * 0.5f;  // Top right

	TeethPos = { DogPos.x - 0.02f, DogPos.y - 0.27 };
	bool isInRange = (TeethPos.x >= BoxMin.x && TeethPos.x <= BoxMax.x) && (TeethPos.y >= BoxMin.y && TeethPos.y <= BoxMax.y);

	if (!m_Down)
	{
		DogPos.y -= 0.02f;
		Puppy1Pos.y -= 0.02f;
		Puppy2Pos.y -= 0.02f;
		Puppy3Pos.y -= 0.02f;

		if (g_Coordinator.HaveComponent<UIComponent>(m_DogHead))
		{
			g_Coordinator.GetComponent<UIComponent>(m_DogHead).set_position(DogPos);
			g_Coordinator.GetComponent<UIComponent>(m_Puppy1).set_position(Puppy1Pos);
			g_Coordinator.GetComponent<UIComponent>(m_Puppy2).set_position(Puppy2Pos);
			g_Coordinator.GetComponent<UIComponent>(m_Puppy3).set_position(Puppy3Pos);
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
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/CorrectSound.wav", false, "SFX");

			// Hit = Pass = Randomize Catchzone position & Faster DogHead Speed.
			m_Speed += 0.2f;

			// Randomize X position (must stay within the smaller range after each hit)
			dist = std::uniform_real_distribution<float>(MinMaxPos.x, MinMaxPos.y);
			CatchZonePos.x = dist(gen);

			// Randomize the X scale (must stay within the smaller range after each hit) - if want to change scale.
			//dist = std::uniform_real_distribution<float>(MinMaxScale.x, CatchZoneScale.x);
			//CatchZoneScale.x = dist(gen);


			if (m_HitCount == m_NoOfHitsRequired)
			{
				g_Coordinator.GetComponent<AnimationComponent>(entity).isBreak = false;
				g_Coordinator.GetComponent<AnimationComponent>(entity).isIdle = false;
				g_Coordinator.GetComponent<AnimationComponent>(entity).isMoving = true;


			/*	std::tuple<int, float, float> animationIdle1 = g_Coordinator.GetComponent<AnimationComponent>(entity).animationVector[g_BoneCatcher.BREAK];

				g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelName()]->SetPlaybackRange(std::get<1>(animationIdle1), std::get<2>(animationIdle1));*/

			}
			else
			{
				g_Coordinator.GetComponent<AnimationComponent>(entity).isBreak = true;
				g_Coordinator.GetComponent<AnimationComponent>(entity).isIdle = false;
				g_Coordinator.GetComponent<AnimationComponent>(entity).isMoving = false;
			//	std::cout << "Mov\n";
				//std::tuple<int, float, float> animationIdle1 = g_Coordinator.GetComponent<AnimationComponent>(entity).animationVector[g_BoneCatcher.MOVE];
				//
				//g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelName()]->SetPlaybackRange(std::get<1>(animationIdle1), std::get<2>(animationIdle1));


			}

		}
		else {
			// Play BOO sound
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO + "/WrongSound.wav", false, "SFX");
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
			Puppy1Pos.y += 0.02f;
			Puppy2Pos.y += 0.02f;
			Puppy3Pos.y += 0.02f;

			if (g_Coordinator.HaveComponent<UIComponent>(m_DogHead))
			{
				g_Coordinator.GetComponent<UIComponent>(m_DogHead).set_position(DogPos);
				g_Coordinator.GetComponent<UIComponent>(m_Puppy1).set_position(Puppy1Pos);
				g_Coordinator.GetComponent<UIComponent>(m_Puppy2).set_position(Puppy2Pos);
				g_Coordinator.GetComponent<UIComponent>(m_Puppy3).set_position(Puppy3Pos);
			}

			if (m_HitDetected)
			{
				if (g_Coordinator.HaveComponent<UIComponent>(m_CatchZone))
				{
					g_Coordinator.GetComponent<UIComponent>(m_CatchZone).set_position(CatchZonePos);
				}
			}

			m_Down = false;
			m_HitDetected = false;
			m_IsMoving = true;

			g_Coordinator.GetComponent<AnimationComponent>(entity).isBreak = false;
	//		g_Coordinator.GetComponent<AnimationComponent>(entity).isIdle = true;
			g_Coordinator.GetComponent<AnimationComponent>(entity).isMoving = false;
//			g_ResourceManager.AnimatorMap[g_Coordinator.GetComponent<GraphicsComponent>(entity).getModelName()]->m_UseCustomRange = false;
		
		}
	}
}
void BoneCatcher::ClearBoneCatcher()
{
	AudioTimer = ClearBoneCatcherTimer;

	// Stop the audio when bonecatcher is cleared
	if (isAudioPlaying)
	{
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/CreakingRope2.wav"); // Stop the specific file path
		g_Audio.StopSpecificSound(FILEPATH_ASSET_AUDIO + "/MetalCage.wav");

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

	storage.clear();
	isActive = false;

	if (UIClosed)
	{
		g_UI.OnInitialize();
		UIClosed = false;
	}
}

void BoneCatcher::ChangeBase(std::string hit2TextureName, std::string hit4TextureName)
{
	if (!m_BaseChanged)
	{
		int firstBreak = (m_NoOfHitsRequired + 1) / 2;
		int secondBreak = m_NoOfHitsRequired - 1;

		if (m_HitCount == firstBreak)
		{
			if (g_Coordinator.HaveComponent<UIComponent>(m_Base))
			{
				g_Coordinator.GetComponent<UIComponent>(m_Base).set_texturename(hit2TextureName);

				if (hit2TextureName == "RopeSemi")
				{
					g_Coordinator.GetComponent<UIComponent>(m_Base).set_scale(glm::vec2(0.7f, 0.16f));
					g_Coordinator.GetComponent<UIComponent>(m_Base).set_position(glm::vec2(0.0f, 0.08f));
				}

				m_BaseChanged = true;
			}
		}
		else if (m_HitCount == secondBreak)
		{
			if (g_Coordinator.HaveComponent<UIComponent>(m_Base))
			{
				g_Coordinator.GetComponent<UIComponent>(m_Base).set_texturename(hit4TextureName);

				if (hit4TextureName == "RopeBreak")
				{
					g_Coordinator.GetComponent<UIComponent>(m_Base).set_scale(glm::vec2(0.7f, 0.16f));
					g_Coordinator.GetComponent<UIComponent>(m_Base).set_position(glm::vec2(0.0f, 0.08f));
				}

				m_BaseChanged = true;
			}
		}
	}
}

void BoneCatcher::UpdatePuppyHeads()
{
	if (!g_Coordinator.HaveComponent<UIComponent>(m_Puppy1) || !g_Coordinator.HaveComponent<UIComponent>(m_Puppy2) || !g_Coordinator.HaveComponent<UIComponent>(m_Puppy3))
		return;

	if (puppyCollisionOrder.size() >= 1) 
	{
		g_Coordinator.GetComponent<UIComponent>(m_Puppy1).set_texturename(puppyTextures[puppyCollisionOrder[0]]);
		g_Coordinator.GetComponent<UIComponent>(m_Puppy1).set_opacity(1.0f);
		m_NoOfHitsRequired = 5;
	}

	if (puppyCollisionOrder.size() >= 2) 
	{
		g_Coordinator.GetComponent<UIComponent>(m_Puppy2).set_texturename(puppyTextures[puppyCollisionOrder[1]]);
		g_Coordinator.GetComponent<UIComponent>(m_Puppy2).set_opacity(1.0f);
		m_NoOfHitsRequired = 4;
	}

	if (puppyCollisionOrder.size() >= 3) 
	{
		g_Coordinator.GetComponent<UIComponent>(m_Puppy3).set_texturename(puppyTextures[puppyCollisionOrder[2]]);
		g_Coordinator.GetComponent<UIComponent>(m_Puppy3).set_opacity(1.0f);
		m_NoOfHitsRequired = 3;
	}
}

void BoneCatcher::ResetBC()
{
	m_HitCount = 0;
	m_Speed = 0.5f;
	m_DestroyTimer = 0.f;
	m_DownTimer = 2.0f;

	m_IsMoving = true;
	m_ShouldDestroy = m_Down = m_Up = m_HitDetected = isAudioPlaying = m_BaseChanged = false;

	m_Direction = 1;
	m_MinPos = -0.335f;
	m_MaxPos = 0.395f;
	ClearBoneCatcherTimer = 0.0;
	AudioTimer = 0.0;

	m_NoOfHitsRequired = 6;
	puppyCollisionOrder.clear();
}
