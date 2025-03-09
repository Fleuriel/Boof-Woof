#include "ChangeText.h"
#include "ResourceManager/ResourceManager.h"
#include "../Checklist/Checklist.h"
#include "../Utilities/ForGame/Dialogue/Dialogue.h"
#include "../Core/AssetManager/FilePaths.h"

ChangeText g_ChangeText;
Serialization serialText;

void ChangeText::OnInitialize()
{
	/*g_DialogueText.OnInitialize();
	shutted = false;*/
}

void ChangeText::OnUpdate(double deltaTime)
{
	//g_DialogueText.OnUpdate(deltaTime);
}

void ChangeText::OnShutdown()
{
	// Just for StartingRoom.h
	/*if (!g_Checklist.shutted && startingRoomOnly)
	{
		g_Checklist.OnInitialize();
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do1");
		g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.15f, 0.05f), "Do2");
		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.15f, 0.05f), "Do3");
		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.15f, 0.05f), "Do4");
	}

	shutted = true;*/
}

void ChangeText::Reset()
{
	shutted = false;
	startingRoomOnly = false;
}
