#pragma once
#ifndef FONTCOMPONENT_HPP
#define FONTCOMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 

class FontComponent {
public:
	FontComponent() {};
	FontComponent(const std::string& family, const glm::vec2& pos, const float& scale, const glm::vec3& color , const std::string& text)
		: family(family), pos(pos), scale(scale), color(color), text(text) {};
	~FontComponent() {};

	// getters
	std::string get_family() const { return family; }
	glm::vec2 get_pos() const { return pos; }
	float get_scale() const { return scale; }
	std::string get_text() const { return text; }
	glm::vec3 get_color() const { return color; }

	// setters
	void set_family(const std::string& family) { this->family = family; }
	void set_pos(const glm::vec2& pos) { this->pos = pos; }
	void set_scale(const float& scale) { this->scale = scale; }
	void set_text(const std::string& text) { this->text = text; }
	void set_color(const glm::vec3& color) { this->color = color; }


	REFLECT_COMPONENT(FontComponent)
	{}

private:
	std::string family = "Arial";
	glm::vec2 pos = { 0.0f, 0.0f };
	float scale = 1.0f;
	std::string text{};
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };
};

#endif // FONTCOMPONENT_HPP
