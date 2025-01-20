#pragma once
#ifndef FONTCOMPONENT_HPP
#define FONTCOMPONENT_HPP

#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp" 

class FontComponent {
public:
	FontComponent() {};
	~FontComponent() {};


	REFLECT_COMPONENT(FontComponent)
	{}

private:
	
};

#endif // FONTCOMPONENT_HPP
