#include "Checklist.h"
#include "ResourceManager/ResourceManager.h"

Checklist g_Checklist;
Serialization serialChecklist;

void Checklist::OnInitialize()
{
	g_SceneManager.LoadScene("../BoofWoof/Assets/Scenes/Checklist.json");
	storage = serialChecklist.GetStored();

	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto entity : entities)
	{
		if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
		{
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
}

void Checklist::OnUpdate(double deltaTime)
{
	if (CheckWASD()) 
	{
		ChangeBoxChecked(Box1);
		Check1 = true;
	}

	if (g_Input.GetKeyState(GLFW_KEY_SPACE) >= 1) 
	{
		ChangeBoxChecked(Box2);
		Check2 = true;
	}

	if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_RIGHT) >= 1) 
	{
		ChangeBoxChecked(Box3);
		Check3 = true;
	}

	if (g_Input.GetKeyState(GLFW_KEY_R) >= 1)
	{
		ChangeBoxChecked(Box4);
		Check4 = true;
	}

	if (Check1 && Check2 && Check3 && Check4) 
	{
		cdTimer += deltaTime;

		if (cdTimer >= cdLimit) 
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
}

void Checklist::ChangeBoxChecked(Entity ent)
{
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
	}
}
