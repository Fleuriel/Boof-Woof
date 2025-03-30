#pragma once

#pragma warning(disable : 4005)
#include <EngineCore.h>

class BoneCatcher
{
public:
	void OnInitialize();
	void OnUpdate(double deltaTime, Entity entity = 2147483647);
	void Stop(double deltaTime, Entity entity = 2147483647);

	void MoveLeftRightVisual(double deltaTime);
	void BiteDown(double deltaTime, Entity entity = 2147483647);	// Check whether collided with catchzone + visual feedback
	void ClearBoneCatcher();
	
	void ChangeBase(std::string hit2TextureName, std::string hit4TextureName);
	void UpdatePuppyHeads();

	void ResetBC();

	enum AnimState
	{
		ANIM_FULLDURATION = 0,
		ANIM_BREAK,
		ANIM_IDLE,
		ANIM_MOVE
	};


public:
	bool isRope{ false }, isCage{ false }, isActive{ false }, savePawgress{ false };
	int m_HitCount{ 0 };
	float m_Speed{ 0.5f };
	int m_Direction{ 1 }; // 1 for right, -1 for left

	// Puppies
	std::vector<int> puppyCollisionOrder;

private:
	Entity m_DogHead{}, m_CatchZone{}, m_Base{};
	Entity m_Puppy1{}, m_Puppy2{}, m_Puppy3{};
	std::unordered_map<int, std::string> puppyTextures = { 
		{1, "DogHeadPuppy1"},
		{2, "DogHeadPuppy2"},
		{3, "DogHeadPuppy3"} 
	};

	// Dog
	glm::vec2 DogPos{}, Puppy1Pos{}, Puppy2Pos{}, Puppy3Pos{};
	float m_DestroyTimer{ 0.f }, m_DownTimer{ 2.0f };
	bool m_IsMoving{ true }, m_ShouldDestroy{ false }, m_Down{ false }, m_Up{ false };
	
	float m_MinPos{ -0.335f }, m_MaxPos{ 0.395f };
	float m_SmollMinPos{ -0.36f }, m_SmollMaxPos{ 0.37f };

	int m_NoOfHitsRequired{ 6 };

	// CatchZone
	glm::vec2 CatchZonePos{};
	glm::vec2 CatchZoneScale{};
	glm::vec2 BoxMin{}, BoxMax{};

	//glm::vec2 MinMaxScale{ 0.1f, 0.4f };
	glm::vec2 MinMaxPos{ -0.35f, 0.37f };	// values of when scale is smallest

	bool m_HitDetected{ false };
	bool m_BaseChanged{ false };

	// Dog Teeth
	glm::vec2 TeethScale{ 0.08f, 0.08f };
	glm::vec2 TeethPos{};

	double ClearBoneCatcherTimer = 0.0;
	double AudioTimer = 0.0;
	bool isAudioPlaying{ false };
	bool UIClosed{ false };

protected:

	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern BoneCatcher g_BoneCatcher;