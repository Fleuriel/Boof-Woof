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
	TUTORIALSTART2,
	FOUNDPUP2,
	ONEMORETOGO,
	TUTORIALSTART3,
	FOUNDPUP3,
	ALLPUPSFOUND,
	STAYCLOSE,
	REXSAWYOU2,
	BREAKROPES,
	BROKEROPE1,
	FREED,
	DISGUSTED2,
	INMAINHALL,
	FIRSTROPEBITE,
	INSTRUCTIONS,
	REXISHERE,
	HIDEFROMREX,
};

class Dialogue
{
public:
	void OnInitialize();
	void OnUpdate(double deltaTime);
	void OnShutdown();
	
	std::string getDialogue();
	void setDialogue(DialogueState newState);
	void ProcessDialogue();

	std::string textWrap(const std::string& input);
	void checkCollision(Entity player, double dt);
	
	void Reset();

public:
	bool dialogueActive{ false };

private:
	Entity m_D1{};
	DialogueState m_CurrentState{ DialogueState::DEFAULT };
	bool NextDialoguePls{ false };
	bool m_TouchedBall{ false }, m_TouchedBone{ false };
	float m_CollisionResetTimer{ 2.0f }; // stunlock timing
	float clickCooldown = 0.0f;

protected:
	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern Dialogue g_DialogueText;