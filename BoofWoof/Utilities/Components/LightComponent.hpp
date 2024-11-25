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
	//void setLightPosition(glm::vec3 l_pos) { LightPosition = l_pos; }

	////// getter
	//glm::vec3 getLightPosition() { return LightPosition; }

	REFLECT_COMPONENT(ParticleComponent)
	{

	}

private:
	//glm::vec3 LightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 color;
	//float intensity;
};



#endif