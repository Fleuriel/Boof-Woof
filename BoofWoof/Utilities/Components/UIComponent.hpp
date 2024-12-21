#pragma once
#ifndef UICOMPONENT_HPP
#define UICOMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 

class UIComponent {
public:
    UIComponent() {};
    ~UIComponent() {};


	void set_topleft(glm::vec2 tl) { topleft = tl; }
	void set_bottomright(glm::vec2 br) { bottomright = br; }
	void set_textureid(int tid) { textureid = tid; }

	glm::vec2 get_topleft() { return topleft; }
	glm::vec2 get_bottomright() { return bottomright; }
	int get_textureid() { return textureid; }


    bool checkclick(glm::vec2 mousepos) {
		if (mousepos.x > topleft.x && mousepos.x < bottomright.x && mousepos.y > topleft.y && mousepos.y < bottomright.y)
			return true;
		return false;
    }
	REFLECT_COMPONENT(UIComponent) 
	{}

private:
    //std::string name;
    int textureid{};
    std::string text{};
	glm::vec2 topleft{ -1.f,1.f };
	glm::vec2 bottomright{ 1.f,-1.f };
};

#endif // UICOMPONENT_HPP
