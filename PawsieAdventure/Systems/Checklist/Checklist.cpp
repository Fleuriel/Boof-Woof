#include "Checklist.h"
#include "ResourceManager/ResourceManager.h"
#include "../ChangeText/ChangeText.h"
#include "../Core/AssetManager/FilePaths.h"

Checklist g_Checklist;
Serialization serialChecklist;

void Checklist::OnInitialize()
{
	g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES+"/Checklist.json");
	storage = serialChecklist.GetStored();

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Do1")
			{
				Do1 = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Do2")
			{
				Do2 = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Do3")
			{
				Do3 = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Do4")
			{
				Do4 = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Box1")
			{
				Box1 = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Box2")
			{
				Box2 = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Box3")
			{
				Box3 = entity;
			}

			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Box4")
			{
				Box4 = entity;
				break;
			}
		}
	}

	shutted = false;
}

void Checklist::OnUpdate(double deltaTime)
{
	if (g_ChangeText.shutted)
	{
		if (CheckWASD() && !Check1)
		{
			ChangeBoxChecked(Box1);
			Check1 = true;
		}

		if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1 && !Check2)
		{
			ChangeBoxChecked(Box2);
			Check2 = true;
		}

		if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) == 1 && !Check3)
		{
			ChangeBoxChecked(Box3);
			Check3 = true;
		}

		if (g_Input.GetKeyState(GLFW_KEY_R) >= 1 && !Check4)
		{
			ChangeBoxChecked(Box4);
			Check4 = true;
		}
	}

	if (Check4 && !corgiText)
	{
		AddCorgiText();
		corgiText = true;
	}

	if ((Check1 && Check2 && Check3 && Check4 && corgiText) || finishTR || finishRB)
	{
		clTimer += deltaTime;

		if (clTimer >= clLimit)
		{
			OnShutdown();
		}
	}
}

bool Checklist::CheckWASD()
{
	if (g_Input.GetKeyState(GLFW_KEY_W) >= 1)
	{
		w = true;
	}

	if (g_Input.GetKeyState(GLFW_KEY_A) >= 1)
	{
		a = true;
	}

	if (g_Input.GetKeyState(GLFW_KEY_S) >= 1)
	{
		s = true;
	}

	if (g_Input.GetKeyState(GLFW_KEY_D) >= 1)
	{
		d = true;
	}

	if (w && a && s && d)
	{
		WASDChecked = true;
	}
	else
	{
		WASDChecked = false;
	}

	return WASDChecked;
}

void Checklist::OnShutdown()
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

	shutted = true;
	clTimer = 0.0;
	Check1 = Check2 = Check3 = Check4 = corgiText = false;
}

void Checklist::ChangeBoxChecked(Entity ent)
{
	playAudio = false;

	if (!g_Coordinator.HaveComponent<GraphicsComponent>(ent)) return;

	auto& text = g_Coordinator.GetComponent<GraphicsComponent>(ent);

	int oldTextureId = g_ResourceManager.GetTextureDDS(text.getTextureName());
	if (text.RemoveTexture(oldTextureId))
	{
		// remove old texture & add new one
		text.clearTextures();

		int textureId = g_ResourceManager.GetTextureDDS("BoxChecked");
		text.AddTexture(textureId);
		text.setTexture("BoxChecked");

		if (!playAudio)
		{
			g_Audio.PlayFileOnNewChannel(FILEPATH_ASSET_AUDIO+"/CheckTheBox.wav", false, "SFX");
			playAudio = true;
		}
	}
}

void Checklist::AddCorgiText()
{
	if (g_ChangeText.shutted)
	{
		g_ChangeText.textureIndex = 6;
		g_ChangeText.indexLimit = 8;
		g_ChangeText.OnInitialize();
		g_ChangeText.shutted = false;
	}
}

void Checklist::ChangeAsset(Entity ent, glm::vec3 scale, std::string textureName)
{
	if (!g_Coordinator.HaveComponent<GraphicsComponent>(ent)) return;

	auto& text = g_Coordinator.GetComponent<GraphicsComponent>(ent);

	// Empty will be -1
	int oldTextureId = g_ResourceManager.GetTextureDDS(text.getTextureName());

	text.RemoveTexture(oldTextureId);

	// remove old texture & add new one
	text.clearTextures();

	int textureId = g_ResourceManager.GetTextureDDS(textureName);
	text.AddTexture(textureId);
	text.setTexture(textureName);

	if (!g_Coordinator.HaveComponent<TransformComponent>(ent)) return;

	g_Coordinator.GetComponent<TransformComponent>(ent).SetScale(scale);

	auto& pos = g_Coordinator.GetComponent<TransformComponent>(ent).GetPosition();

	if (textureName == "Do6")
	{
		g_Coordinator.GetComponent<TransformComponent>(ent).SetPosition(glm::vec3(pos.x + 0.02f, pos.y, pos.z));
	}

	if (textureName == "Do7")
	{
		g_Coordinator.GetComponent<TransformComponent>(ent).SetPosition(glm::vec3(pos.x, pos.y - 0.01f, pos.z));
	}

	if (textureName == "Do8")
	{
		g_Coordinator.GetComponent<TransformComponent>(ent).SetPosition(glm::vec3(pos.x - 0.02f, pos.y, pos.z));
	}
}
