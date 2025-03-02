#pragma once

#pragma warning(disable : 4005)
#include <EngineCore.h>

class BoneCatcher
{
public:
	void OnInitialize();
	void OnUpdate(double deltaTime);
	void Stop(double deltaTime);

	void MoveLeftRightVisual(double deltaTime);
	void BiteDown(double deltaTime);	// Check whether collided with catchzone + visual feedback
	void ClearBoneCatcher();
	void ChangeBase(std::string hit2TextureName, std::string hit4TextureName);


private:
	Entity m_DogHead{}, m_CatchZone{}, m_Base{};
	int m_HitCount{ 0 };

	// Dog
	glm::vec3 initialDogPos{}, DogPos{}, DogScale{};
	float m_Speed{ 0.5f }, m_DestroyTimer{ 0.f }, m_DownTimer{};
	bool m_IsMoving{ true }, m_ShouldDestroy{ false }, m_Down{ false }, m_Up{ false };
	int m_Direction{ 1 }; // 1 for right, -1 for left
	float m_MinPos{ -0.23f }, m_MaxPos{ 0.27f };

	// CatchZone
	glm::vec3 CatchZonePos{};
	glm::vec3 CatchZoneScale{};
	glm::vec3 BoxMin{}, BoxMax{};

	//glm::vec2 MinMaxScale{ 0.1f, 0.4f };
	glm::vec2 MinMaxPos{ -0.24f, 0.26f };	// values of when scale is smallest

	bool m_HitDetected{ false };
	bool m_BaseChanged{ false };

	// Dog Teeth
	glm::vec3 TeethScale{ 0.07f, 0.1f, 0.f };
	glm::vec3 TeethPos{};

	double ClearBoneCatcherTimer = 0.0;
	double AudioTimer = 0.0;
	bool isAudioPlaying{ false };

protected:

	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern BoneCatcher g_BoneCatcher;