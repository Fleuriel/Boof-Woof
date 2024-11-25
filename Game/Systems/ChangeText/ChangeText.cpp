#include "ChangeText.h"
#include "ResourceManager/ResourceManager.h"

ChangeText g_ChangeText;
Serialization serialText;

void ChangeText::OnInitialize()
{
	g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/StartingText.json");
	storage = serialText.GetStored();

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Text")
			{
				m_Text = entity;
				break;
			}			
		}
	}
}

void ChangeText::OnUpdate(double deltaTime)
{
	cdTimer += deltaTime;

	if (!g_Coordinator.HaveComponent<GraphicsComponent>(m_Text)) return;

	auto& text = g_Coordinator.GetComponent<GraphicsComponent>(m_Text);

	// If it's time to change the texture
	if (cdTimer >= cdLimit && textureIndex < 6) // Only switch to CorgiText1, CorgiText2, CorgiText3, CorgiText4
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

	switch (textureIndex)
	{
	case 5:
	{
		// First i need to get out of this place
		g_Audio.PlayFile("../BoofWoof/Assets/Audio/AggressiveDogBarking.wav");
		break;
	}
	case 6:
	{
		// One step at a time
		if (cdTimer <= 3.0)
		{
			g_Audio.PlayFile("../BoofWoof/Assets/Audio/CorgiWhimper.wav");
		}
		break;
	}
	}


	// After showing CorgiText6 for 3 seconds, clear entity
	if (textureIndex >= 6 && cdTimer >= cdLimit)
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
}