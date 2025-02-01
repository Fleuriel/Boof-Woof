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

	
    // if mouse click
    if (g_Input.GetMouseState(0) == 1) {
        for (auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
            if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "folder UI") {
                if (g_Coordinator.HaveComponent<UIComponent>(entity)) {
                    auto& UICompt = g_Coordinator.GetComponent<UIComponent>(entity);
                    if (UICompt.get_selected()) {
                        std::cout << "folder UI is clicked \n";
                    }
                }
            }
        }
    }

    // Use GetDeltaTime() instead of GetElapsedTime()
    float currentTime = static_cast<float>(g_Window->GetDeltaTime()); // This gives you the time since the last frame

    // Loop through all UI components
    for (auto& entity : g_Coordinator.GetAliveEntitiesSet()) {
        if (g_Coordinator.HaveComponent<UIComponent>(entity)) {
            auto& UICompt = g_Coordinator.GetComponent<UIComponent>(entity);

            if (UICompt.get_playing()){
                // Update the timer
                UICompt.set_timer(UICompt.get_timer() + currentTime);

                // Check if it's time to update the animation frame
                if (UICompt.get_timer() >= UICompt.get_frame_interval()) {
                    // Increment the current row/column for animation
                    int curr_row = UICompt.get_curr_row();
                    int curr_col = UICompt.get_curr_col();
                    int rows = UICompt.get_rows();
                    int cols = UICompt.get_cols();
                    bool stayOnRow = UICompt.get_stay_on_row();

                    // Handle animation logic
                    if (stayOnRow) {
                        // Increment column, reset if it exceeds max
                        curr_col = (curr_col + 1) % cols;
                    }
                    else {
                        // Increment column, and move to the next row if needed
                        if (++curr_col >= cols) {
                            curr_col = 0;
                            if (++curr_row >= rows) {
                                curr_row = 0;  // Reset row after exceeding max rows
                            }
                        }
                    }

                    // Update current row/column
                    UICompt.set_curr_row(curr_row);
                    UICompt.set_curr_col(curr_col);

                    // Reset the timer
                    UICompt.set_timer(0.f);
                }
            }

            UI_render();

        }
    }

    //glEnable(GL_DEPTH_TEST);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void UISystem::UI_render()
{
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

            // Apply translation
            model = glm::translate(model, UI_pos);
            // Apply rotation around the Z-axis.
            // We use glm::radians() to convert the rotation angle from degrees to radians.
            model = glm::rotate(model, glm::radians(UICompt.get_rotation()), glm::vec3(0.0f, 0.0f, 1.0f));
            // Apply scaling
            model = glm::scale(model, UI_scale);

            // Render non-animated UI with the default shader:
            if (UICompt.get_animate() == false)
            {
                auto& shader = g_AssetManager.GetShader("Shader2D");
                shader.Use();

                shader.SetUniform("vertexTransform", model);
                shader.SetUniform("view", glm::mat4(1.0f));
                shader.SetUniform("projection", glm::mat4(1.0f));
                shader.SetUniform("opacity", UICompt.get_UI_opacity());

                // Bind texture               
                glBindTextureUnit(6, g_ResourceManager.GetTextureDDS(UICompt.get_texturename()));
                shader.SetUniform("uTex2d", 6);

                g_ResourceManager.getModel("Square")->Draw2D(shader);
                shader.UnUse();
            }
            else
            {
                // Animated UI branch remains similar but can include rotation in the transformation.
                auto& shader = g_AssetManager.GetShader("Sprite");
                shader.Use();

                glm::vec3 UI_pos1 = { UICompt.get_position(), UICompt.get_UI_layer() };
                glm::vec3 UI_scale1 = { UICompt.get_scale(), 1.0f };

                glm::mat4 model1 = glm::mat4(1.0f);
                model1 = glm::translate(model1, UI_pos1);
                model1 = glm::rotate(model1, glm::radians(UICompt.get_rotation()), glm::vec3(0.0f, 0.0f, 1.0f));
                model1 = glm::scale(model1, UI_scale1);

                // Here we send the 3x3 matrix for 2D transformations.
                shader.SetUniform("uModel_to_NDC", glm::mat3(model1));

                shader.SetUniform("opacity", UICompt.get_UI_opacity());
                shader.SetUniform("rows", UICompt.get_rows());
                shader.SetUniform("cols", UICompt.get_cols());
                shader.SetUniform("row_To_Draw", UICompt.get_curr_row());
                shader.SetUniform("col_To_Draw", UICompt.get_curr_col());

                // Bind texture
                glBindTextureUnit(6, g_ResourceManager.GetTextureDDS(UICompt.get_texturename()));
                shader.SetUniform("uTex2d", 6);

                g_ResourceManager.getModel("Square")->Draw2D(shader);
                shader.UnUse();
            }
        }
    }
}
