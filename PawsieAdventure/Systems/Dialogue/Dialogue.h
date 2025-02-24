#pragma once
#include <EngineCore.h>

enum class DialogueState {
	DEFAULT,
	TUTORIALSTART,
	TUTORIALEND,
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
	/*void setDialogue(DialogueState newState, Entity entity);*/
	void setDialogue(DialogueState newState);
	void Reset();

private:
	Entity m_D1{};
	DialogueState m_CurrentState{ DialogueState::DEFAULT };

protected:
	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern Dialogue g_DialogueText;