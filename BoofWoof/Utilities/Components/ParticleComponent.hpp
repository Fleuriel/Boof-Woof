#pragma once
#ifndef PARTICLE_COMPONENT_H
#define PARTICLE_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"

class ParticleComponent
{
public:
	void Update() {};
	ParticleComponent() {};
	~ParticleComponent() {};

	REFLECT_COMPONENT(ParticleComponent)
	{

	}


private:

};



#endif