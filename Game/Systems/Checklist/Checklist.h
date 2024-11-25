#pragma once
#include <EngineCore.h>

class Checklist
{
public:
	void OnInitialize();
	void OnUpdate(double deltaTime);
	bool CheckWASD();
	void OnShutdown();
	void ChangeBoxChecked(Entity ent);

private:
	Entity Box1{}, Box2{}, Box3{}, Box4{};

	bool w{ false }, a{ false }, s{ false }, d{ false };
	bool WASDChecked{ false };
	bool Check1{ false }, Check2{ false }, Check3{ false }, Check4{ false };
	bool playAudio{ false };

	double clTimer = 0.0;
	double clLimit = 4.0;

protected:

	// Storage is for those entity you added in when you load a scene file
	// during exit, can just delete those entity - clearing spawned entities
	std::vector<Entity> storage;
};

extern Checklist g_Checklist;