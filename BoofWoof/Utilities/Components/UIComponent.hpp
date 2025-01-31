#pragma once
#ifndef UICOMPONENT_HPP
#define UICOMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 


class UIComponent {
public:
	UIComponent() {};
	UIComponent(std::string tex , glm::vec2 pos, glm::vec2 s, float layer, bool selecable, float opacity) : texturename(tex), position(pos), scale(s), layer(layer), selectable(selecable), UI_opacity(opacity) {};
	~UIComponent() {};

	// setters
	void set_position(glm::vec2 pos) { position = pos; }
	void set_scale(glm::vec2 s) { scale = s; }
	void set_texturename(std::string t) {	texturename = t;}
	void set_UI_layer(float l) { layer = l; }
	void set_opacity(float o) { UI_opacity = o; }
	void set_selectable(bool s) { selectable = s; }
	void set_animate(bool a) { animated = a; }
	void set_rows(int r) { rows = r; }
	void set_cols(int c) { cols = c; }
	void set_curr_row(int r) { curr_row = r; }
	void set_curr_col(int c) { curr_col = c; }
	void set_frame_interval(float i) { frame_interval = i; }
	void set_timer(float t) { timer = t; }
	void set_playing(bool p) { playing = p; }
	void set_stay_on_row(bool s) { stay_on_row = s; }

	// getters
	glm::vec2 get_position() { return position; }
	glm::vec2 get_scale() { return scale; }
	std::string get_texturename() { return texturename; }
	float get_UI_layer() const { return layer; }
	float get_opacity() const { return UI_opacity; }
	bool get_selectable() const { return selectable; }
	bool get_animate() const { return animated; }
	int get_rows() const { return rows; }
	int get_cols() const { return cols; }
	int get_curr_row() const { return curr_row; }
	int get_curr_col() const { return curr_col; }
	float get_frame_interval() const { return frame_interval; }
	float get_timer() const { return timer; }
	bool get_playing() const { return playing; }
	bool get_stay_on_row() const { return stay_on_row; }

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
			UI_opacity = 0.8f;
		}
		else {
			selected = false;
			UI_opacity = 1.f;
		}
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

private:
	//std::string name;
	std::string texturename;
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
	int curr_row{ 1 };
	int curr_col{ 1 };
	float frame_interval{ 0.1f };
	float timer{ 0.f };
	bool playing{ true };
	bool stay_on_row{ true };
};

#endif // UICOMPONENT_HPP
