#include "ChangeText.h"
#include "ResourceManager/ResourceManager.h"
#include "../Checklist/Checklist.h"
#include "../Core/AssetManager/FilePaths.h"

#include "ChangeText.h"
#include "ResourceManager/ResourceManager.h"
#include "../Checklist/Checklist.h"

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

	if (!g_Coordinator.HaveComponent<GraphicsComponent>(m_Text)) return;

	auto& text = g_Coordinator.GetComponent<GraphicsComponent>(m_Text);

	// If it's time to change the texture
	if (cdTimer >= cdLimit && textureIndex < indexLimit)
	{
		std::string newTextureName = "CorgiText" + std::to_string(textureIndex + 1);

		int oldTextureId = g_ResourceManager.GetTextureDDS(text.getTextureName());
		if (text.RemoveTexture(oldTextureId))
		{
			// remove old texture & add new one
			text.clearTextures();

			int textureId = g_ResourceManager.GetTextureDDS(newTextureName);
			text.AddTexture(textureId);
			text.setTexture(newTextureName);
		}

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
	if (!g_Checklist.shutted)
	{
		g_Checklist.OnInitialize();
		g_Checklist.ChangeAsset(g_Checklist.Do1, glm::vec3(0.25f, 0.08f, 1.0f), "Do1");
		g_Checklist.ChangeAsset(g_Checklist.Do2, glm::vec3(0.25f, 0.09f, 1.0f), "Do2");
		g_Checklist.ChangeAsset(g_Checklist.Do3, glm::vec3(0.25f, 0.075f, 1.0f), "Do3");
		g_Checklist.ChangeAsset(g_Checklist.Do4, glm::vec3(0.26f, 0.08f, 1.0f), "Do4");
	}

	shutted = true;
}
