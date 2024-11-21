#pragma once
#include <EngineCore.h>

class BoneCatcher
{
public:
	void OnLoad();
	void OnInitialize();
	void OnUpdate(double deltaTime);
	void Stop();

	void ClearBoneCatcher();

private:
	Entity m_DogHead{};
	glm::vec3 initialDogPos{};
	float m_Speed{ 2.5f }, m_DestroyTimer{ 0.f };
	bool m_IsMoving{ true }, m_ShouldDestroy{ false };
	int m_Direction{ 1 }; // 1 for right, -1 for left
	float m_MinPos{ -1.9f }, m_MaxPos{ 2.2f };

protected:

	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern BoneCatcher g_BoneCatcher;