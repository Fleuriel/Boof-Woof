#pragma once
#pragma warning(disable : 4005)
#include <EngineCore.h>

class Checklist
{
public:
	void OnInitialize();
	void OnUpdate(double deltaTime);
	bool CheckWASD();
	void OnShutdown();
	void ChangeBoxChecked(Entity ent);
	void AddCorgiText();
	void Reset();

	void HideChecklistUI(std::vector<Entity> ent, bool hide);
	void ChangeAsset(Entity ent, glm::vec2 scale, std::string textureName);

public:
	bool shutted{ false }, finishTR{ false }, finishRB{ false }, Check3{ false };
	Entity Do1{}, Do2{}, Do3{}, Do4{};
	Entity Box1{}, Box2{}, Box3{}, Box4{};
	Entity Paper{};

private:
	bool w{ false }, a{ false }, s{ false }, d{ false };
	bool WASDChecked{ false };
	bool Check1{ false }, Check2{ false }, Check4{ false };
	bool playAudio{ false };

	double clTimer = 0.0;
	double clLimit = 2.0;

	bool corgiText{ false };

protected:
	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern Checklist g_Checklist;