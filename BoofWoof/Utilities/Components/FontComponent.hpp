#pragma once
#ifndef FONTCOMPONENT_HPP
#define FONTCOMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 

class FontComponent {
public:
	FontComponent() {};
	FontComponent(const std::string& family, const glm::vec2& pos, const glm::vec2& scale, const glm::vec3& color , const std::string& text)
		: family(family), pos(pos), scale(scale), color(color), text(text) {};
	~FontComponent() {};

	// getters
	std::string get_family() const { return family; }
	glm::vec2 get_pos() const { return pos; }
	glm::vec2 get_scale() const { return scale; }
	std::string get_text() const { return text; }
	glm::vec3 get_color() const { return color; }

	// setters
	void set_family(const std::string& fam) { this->family = fam; }
	void set_pos(const glm::vec2& p) { this->pos = p; }
	void set_scale(const glm::vec2& s) { this->scale = s; }
	void set_text(const std::string& txt) { this->text = txt; }
	void set_color(const glm::vec3& clr) { this->color = clr; }


	REFLECT_COMPONENT(FontComponent)
	{}

private:
	std::string family = "arial";
	glm::vec2 pos = { 0.0f, 0.0f };
	glm::vec2 scale = { 1.0f, 1.0f };
	std::string text{};
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };
};

#endif // FONTCOMPONENT_HPP
