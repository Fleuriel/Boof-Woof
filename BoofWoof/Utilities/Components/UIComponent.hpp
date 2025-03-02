#pragma once
#ifndef UICOMPONENT_HPP
#define UICOMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 

class UIComponent {
public:
    UIComponent() {};
	UIComponent(int tid, glm::vec2 pos, glm::vec2 s) : textureid(tid), position(pos), scale(s) {};
    ~UIComponent() {};


	void set_position(glm::vec2 pos) { position = pos; }
	void set_scale(glm::vec2 s) { scale = s; }
	void set_textureid(int tid) { textureid = tid; }

	glm::vec2 get_position() { return position; }
	glm::vec2 get_scale() { return scale; }
	bool get_selected() { return selected; }
	float get_UI_opacity() { return UI_opacity; }
	int get_textureid() { return textureid; }


<<<<<<< HEAD

    void checkclick(glm::vec2 mousepos) {
=======
	bool hasPlayedSound = false;

    // UI interaction
    bool get_selected() { return selected; }
    float get_UI_opacity() { return UI_opacity; }

	bool checkclick(glm::vec2 mousepos) {
		if (!selectable) return false;

>>>>>>> main
		float left_limit = position.x - scale.x;
		float right_limit = position.x + scale.x;
		float top_limit = position.y + scale.y;
		float bottom_limit = position.y - scale.y;

		if (mousepos.x > left_limit && mousepos.x < right_limit &&
			mousepos.y < top_limit && mousepos.y > bottom_limit) {

			if (!hasPlayedSound) { // Play sound only once
				hasPlayedSound = true;
				return true; // Indicates sound should play
			}
			selected = true;
			UI_opacity = 0.5f;
		}
		else {
			selected = false;
			UI_opacity = 1.f;
			hasPlayedSound = false; // Reset when mouse leaves
		}
<<<<<<< HEAD
    }
	REFLECT_COMPONENT(UIComponent) 
	{}
=======
		return false;
	}


	REFLECT_COMPONENT(UIComponent)
	{
		REGISTER_PROPERTY(UIComponent, TextureName, std::string, set_texturename, get_texturename);
		REGISTER_PROPERTY(UIComponent, Position, glm::vec2, set_position, get_position);
		REGISTER_PROPERTY(UIComponent, Scale, glm::vec2, set_scale, get_scale);
		REGISTER_PROPERTY(UIComponent, Layer, float, set_UI_layer, get_UI_layer);
		REGISTER_PROPERTY(UIComponent, Selectable, bool, set_selectable, get_selectable);
		REGISTER_PROPERTY(UIComponent, Opacity, float, set_opacity, get_opacity);
	}
>>>>>>> main

private:
    //std::string name;
    int textureid{};
    std::string text{};

	glm::vec2 position{ 0.f, 0.f };
	glm::vec2 scale{ 1.f, 1.f };

<<<<<<< HEAD
	bool selected{ false };
	float UI_opacity{ 1.f };
=======
    float layer = 0.5f;
    bool selectable{ false };
    float UI_opacity{ 1.f };

    bool selected{ false };

    // NEW: Add a rotation property
    float rotation = 0.0f;
	bool animated{ false };
	int rows{ 1 };
	int cols{ 1 };
	int curr_row{ 1 };
	int curr_col{ 0 };
	float frame_interval{ 0.1f };
	float timer{ 0.f };
	bool playing{ false };
	bool stay_on_row{ true };
>>>>>>> main
};

#endif // UICOMPONENT_HPP
