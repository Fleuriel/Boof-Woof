#pragma once
#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"


class LightComponent
{
public:
	LightComponent()
		: intensity(1.0f), color(glm::vec3(1.0f, 1.0f, 1.0f)), haveshadow(false), direction(glm::vec3(1.0f, 0.0f, 0.0f)) {}

	LightComponent(float i, glm::vec3 c, bool shadow, glm::vec3 dir)
		: intensity(i), color(c), haveshadow(shadow), direction(dir) {}

	~LightComponent() {}

	////// setter
	void setIntensity(float i) { intensity = i; }
	void setColor(glm::vec3 c) { color = c; }
	void setShadow(bool s) { haveshadow = s; }
	void setDirection(glm::vec3 d) { direction = d; }

	////// getter
	float getIntensity() const { return intensity; }
	glm::vec3 getColor() const { return color; }
	bool getShadow() const { return haveshadow; }
	glm::vec3 getDirection() const { return direction; }

	REFLECT_COMPONENT(LightComponent)
	{

	}

private:
	float intensity = 1.0f;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	bool haveshadow = false;
	glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
};



#endif