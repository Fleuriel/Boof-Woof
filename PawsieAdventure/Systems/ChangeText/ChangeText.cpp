#include "ChangeText.h"
#include "ResourceManager/ResourceManager.h"
#include "../Checklist/Checklist.h"
#include "../Core/AssetManager/FilePaths.h"

ChangeText g_ChangeText;
Serialization serialText;

void ChangeText::OnInitialize()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/StartingText.json");
	storage = serialText.GetStored();

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "StartingText")
			{
				m_Text = entity;
				break;
			}
		}
	}

	shutted = false;
}

void ChangeText::OnUpdate(double deltaTime)
{
	cdTimer += deltaTime;

	if (!g_Coordinator.HaveComponent<UIComponent>(m_Text)) return;

	auto& text = g_Coordinator.GetComponent<UIComponent>(m_Text);

	// If it's time to change the texture
	if (cdTimer >= cdLimit && textureIndex < indexLimit)
	{
		std::string newTextureName = "CorgiText" + std::to_string(textureIndex + 1);

		//int oldTextureId = text.get_textureid();
		//int textureId = g_ResourceManager.GetTextureDDS(newTextureName);
		//text.set_textureid(textureId);
		text.set_texturename(newTextureName);

		cdTimer = 0.0;  // Reset the timer after each texture change
		textureIndex++;  // Move to the next texture
	}

	// After showing CorgiText6 for 3 seconds, clear entity
	if (textureIndex >= indexLimit && cdTimer >= cdLimit)
	{
		OnShutdown();
	}
}

void ChangeText::OnShutdown()
{
	// Just remove whatever we had stored from the current alive entity and destroy them
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto i = entities.begin(); i != entities.end(); i++)
	{
		for (auto k = storage.begin(); k != storage.end(); k++)
		{
			if (*k == *i)
			{
				g_Coordinator.DestroyEntity(*i);
			}
		}
	}

	// Just for StartingRoom.h
	if (!g_Checklist.shutted && startingRoomOnly)
	{
		g_Checklist.OnInitialize();
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec2(0.15f, 0.05f), "Do1");
		g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec2(0.15f, 0.05f), "Do2");
		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec2(0.15f, 0.05f), "Do3");
		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec2(0.15f, 0.05f), "Do4");
	}

	shutted = true;
}

void ChangeText::Reset()
{
	shutted = false;
	textureIndex = 5;
	indexLimit = 6;
	startingRoomOnly = false;
}
