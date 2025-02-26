#pragma once
#include <EngineCore.h>

enum class DialogueState {
	DEFAULT,
	TUTORIALSTART,
	TUTORIALEND,
	ENTEREDLIBRARY,
	TOUCHBALL,
	TOUCHBONE,
	DONTWASTETIME,
	OUTOFLIBRARY,
	REXSAWYOU,
	SEARCHINGFORPUPS,
	FOUNDPUP1,
	TWOMORETOGO,
	DISGUSTED,
	SEARCHINGFORPUPS2,
	FOUNDPUP2,
	ONEMORETOGO,
	SEARCHINGFORPUPS3,
	FOUNDPUP3,
	ALLPUPSFOUND,
	STAYCLOSE,
	REXSAWYOU2,
	BREAKROPES,
	BROKEROPE1,
	FREED,
};

class Dialogue
{
public:
	void OnInitialize();
	void OnUpdate(double deltaTime);
	void OnShutdown();
	
	std::string getDialogue();
	void setDialogue(DialogueState newState);
	
	std::string textWrap(const std::string& input);
	void checkCollision(Entity player, double dt);
	
	void Reset();


private:
	Entity m_D1{};
	DialogueState m_CurrentState{ DialogueState::DEFAULT };
	bool m_DialogueActive{ false }, m_FirstTimeTouchBall{ true }, m_FirstTimeTouchBone{ true }, m_TouchedBall{ false }, m_TouchedBone{ false };
	float m_CollisionResetTimer{ 2.0f }; // stunlock timing

protected:
	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern Dialogue g_DialogueText;