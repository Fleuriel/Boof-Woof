#pragma once
#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"


class LightComponent
{
public:
	LightComponent() {};
	//LightComponent(glm::vec3 l_pos) :LightPosition(l_pos) {}
	~LightComponent() {
	};

	////// setter
	void setIntensity(float i) { intensity = i; }

	////// getter
	float getIntensity() const { return intensity; }

	REFLECT_COMPONENT(ParticleComponent)
	{

	}

private:
	//glm::vec3 LightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 color;
	float intensity = 1.0f;
};



#endif