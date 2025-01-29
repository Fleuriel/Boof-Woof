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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_DEPTH_TEST);
	
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
	g_AssetManager.GetShader("Shader2D").Use();
	UI_render();
	g_AssetManager.GetShader("Shader2D").UnUse();

	//glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UISystem::UI_render()
{
    // Render UI
    for (auto& entity : g_Coordinator.GetAliveEntitiesSet())
    {
        if (g_Coordinator.HaveComponent<UIComponent>(entity))
        {
            auto& UICompt = g_Coordinator.GetComponent<UIComponent>(entity);

            glm::vec2 mouse_pos = {
                (g_Input.GetMousePositionUI().x / g_WindowX) * 2.0f - 1.0f,
                1.0f - 2.0f * (g_Input.GetMousePositionUI().y / g_WindowY)
            };
            UICompt.checkclick(mouse_pos);

            // Set up model matrix
            glm::vec3 UI_pos = { UICompt.get_position(), UICompt.get_UI_layer() };
            glm::vec3 UI_scale = { UICompt.get_scale(), 1.f };

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, UI_pos);
            model = glm::scale(model, { UI_scale.x, UI_scale.y, 1.0f });

            if (UICompt.get_animate() == false)
            {
                // Use default shader
                auto& shader = g_AssetManager.GetShader("Shader2D");
                shader.SetUniform("vertexTransform", model);
                shader.SetUniform("view", glm::mat4(1.0f));
                shader.SetUniform("projection", glm::mat4(1.0f));
                shader.SetUniform("opacity", UICompt.get_UI_opacity());
                glBindTextureUnit(6, UICompt.get_textureid());
                shader.SetUniform("uTex2d", 6);
                g_ResourceManager.getModel("Square")->Draw2D(shader);
            }
            else
            {
                // Use new sprite shader
                auto& spriteShader = g_AssetManager.GetShader("Sprite"); // Load your shader
                spriteShader.Use();

                spriteShader.SetUniform("vertexTransform", model);
                spriteShader.SetUniform("view", glm::mat4(1.0f));
                spriteShader.SetUniform("projection", glm::mat4(1.0f));
                spriteShader.SetUniform("opacity", UICompt.get_UI_opacity());

                // Bind texture for sprite shader
                glBindTextureUnit(7, UICompt.get_textureid()); // Different texture unit if needed
                spriteShader.SetUniform("uTex2d", 7);

                g_ResourceManager.getModel("Square")->Draw2D(spriteShader);
            }
        }
    }
}


