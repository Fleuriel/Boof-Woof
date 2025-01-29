#pragma once
#ifndef UICOMPONENT_HPP
#define UICOMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 

class UIComponent {
public:
	UIComponent() {};
	UIComponent(int tid, glm::vec2 pos, glm::vec2 s, float layer, bool selecable, float opacity) : textureid(tid), position(pos), scale(s), layer(layer), selectable(selecable), UI_opacity(opacity) {};
	~UIComponent() {};

	// setters
	void set_position(glm::vec2 pos) { position = pos; }
	void set_scale(glm::vec2 s) { scale = s; }
	void set_textureid(int tid) { textureid = tid; }
	void set_UI_layer(float l) { layer = l; }
	void set_opacity(float o) { UI_opacity = o; }
	void set_selectable(bool s) { selectable = s; }
	void set_animate(bool a) { animated = a; }
	void set_rows(int r) { rows = r; }
	void set_cols(int c) { cols = c; }

	// getters
	glm::vec2 get_position() { return position; }
	glm::vec2 get_scale() { return scale; }
	int get_textureid() const { return textureid; }
	float get_UI_layer() const { return layer; }
	float get_opacity() const { return UI_opacity; }
	bool get_selectable() const { return selectable; }
	bool get_animate() const { return animated; }
	int get_rows() const { return rows; }
	int get_cols() const { return cols; }


	// UI interaction
	bool get_selected() { return selected; }
	float get_UI_opacity() { return UI_opacity; }


	void checkclick(glm::vec2 mousepos) {
		if (!selectable) return;
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

	float layer = 0.5f;
	bool selectable{ false };
	float UI_opacity{ 1.f };


	bool selected{ false };

	bool animated{ false };
	int rows{ 1 };
	int cols{ 1 };
};

#endif // UICOMPONENT_HPP
