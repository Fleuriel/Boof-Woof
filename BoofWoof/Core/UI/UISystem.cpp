#include "pch.h"
#include "UISystem.h"

#include "../Utilities/Components/UIComponent.hpp"
#include "../Utilities/Components/TransformComponent.hpp"
#include "../Utilities/Components/MetaData.hpp"

#include "../Graphics/FontSystem.h"
#include "AssetManager/AssetManager.h"
#include "ResourceManager/ResourceManager.h"
#include "Windows/WindowManager.h"

#include "Input/Input.h"

UISystem uiSystem;

void UISystem::UI_init() {
	std::cout << "UI System Init\n";
}

void UISystem::UI_update() {
	// if mouse click
	if (g_Input.GetMouseState(0) == 1) {
		for (auto& entity : g_Coordinator.GetAliveEntitiesSet())
		{
			if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "folder UI")
			{
				if (g_Coordinator.HaveComponent<UIComponent>(entity)) {
					auto& UICompt = g_Coordinator.GetComponent<UIComponent>(entity);
					if (UICompt.get_selected()) {
						std::cout << "folder UI is clicked \n";
					}
				}
			}
		}
	}

	UI_render();
}

void UISystem::UI_render()
{
	//render UI
	g_AssetManager.GetShader("Shader2D").Use();
	for (auto& entity : g_Coordinator.GetAliveEntitiesSet())
	{
		if (g_Coordinator.HaveComponent<UIComponent>(entity)) {
			auto& UICompt = g_Coordinator.GetComponent<UIComponent>(entity);

			auto& transCompt = g_Coordinator.GetComponent<TransformComponent>(entity);

			transCompt.SetPosition({ UICompt.get_position() , 0.f});
			transCompt.SetScale({ UICompt.get_scale() , 1.f});

			glm::vec2 mouse_pos = { (g_Input.GetMousePositionUI().x / g_WindowX) * 2.0f - 1.0f, 1.0f - 2.0f * (g_Input.GetMousePositionUI().y / g_WindowY) };

			UICompt.checkclick(mouse_pos);

			// call 2d render
			g_AssetManager.GetShader("Shader2D").SetUniform("vertexTransform", transCompt.GetWorldMatrix());
			g_AssetManager.GetShader("Shader2D").SetUniform("view", glm::mat4(1.0f));
			g_AssetManager.GetShader("Shader2D").SetUniform("projection", glm::mat4(1.0f));
			g_AssetManager.GetShader("Shader2D").SetUniform("opacity", UICompt.get_UI_opacity());
			glBindTextureUnit(6, UICompt.get_textureid());

			g_AssetManager.GetShader("Shader2D").SetUniform("uTex2d", 6);

			g_ResourceManager.getModel("Square")->Draw2D(g_AssetManager.GetShader("Shader2D"));
		}
	}

	g_AssetManager.GetShader("Shader2D").UnUse();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


