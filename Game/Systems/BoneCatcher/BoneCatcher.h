#pragma once
#include <EngineCore.h>

class BoneCatcher
{
public:
	void OnLoad();
	void OnInitialize();
	void OnUpdate(double deltaTime);
	void Stop();

	void MoveLeftRightVisual(double deltaTime);
	void BiteDownVisual(double deltaTime);
	void ClearBoneCatcher();

private:
	Entity m_DogHead{}, m_CatchZone{};

	// Dog
	glm::vec3 initialDogPos{}, DogPos{}, DogScale{};
	float m_Speed{ 2.5f }, m_DestroyTimer{ 0.f }, m_DownTimer{};
	bool m_IsMoving{ true }, m_ShouldDestroy{ false }, m_Down{ false }, m_Up{ false };
	int m_Direction{ 1 }; // 1 for right, -1 for left
	float m_MinPos{ -1.9f }, m_MaxPos{ 2.2f };

	// CatchZone
	glm::vec3 CatchZonePos{};
	glm::vec3 CatchZoneScale{};
	glm::vec3 BoxMin{}, BoxMax{};

	glm::vec2 MinMaxScale{ 0.7, 4.0f };
	glm::vec2 MinMaxPos{ -1.4f, 2.0f };

	bool m_HitDetected{ false };

	// Dog Teeth
	glm::vec3 TeethScale{ 0.5f, 0.2f, 0.f };
	glm::vec3 TeethPos{};

protected:

	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern BoneCatcher g_BoneCatcher;