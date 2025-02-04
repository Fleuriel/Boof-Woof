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



    void checkclick(glm::vec2 mousepos) {
		float left_limit = position.x - scale.x;
		float right_limit = position.x + scale.x;
		float top_limit = position.y + scale.y;
		float bottom_limit = position.y - scale.y;
		if (mousepos.x > left_limit && mousepos.x < right_limit && mousepos.y < top_limit && mousepos.y > bottom_limit) {
			selected = true;
			UI_opacity = 0.5f;
		}
		else {
			selected = false;
			UI_opacity = 1.f;
		}
    }
	REFLECT_COMPONENT(UIComponent) 
	{}

private:
    //std::string name;
    int textureid{};
    std::string text{};

	glm::vec2 position{ 0.f, 0.f };
	glm::vec2 scale{ 1.f, 1.f };

	bool selected{ false };
	float UI_opacity{ 1.f };
};

#endif // UICOMPONENT_HPP
