#pragma once
#ifndef PARTICLE_COMPONENT_H
#define PARTICLE_COMPONENT_H


#include "ECS/Coordinator.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"
#include "../Core/Reflection/ReflectionManager.hpp"

#define PARTICLE_NUM 100

class ParticleComponent
{
public:
	ParticleComponent() {};
	~ParticleComponent() {};

	class particle
	{
	public:
		particle() {};
		~particle() {};
		
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 direction;
		float lifeTime;
		float lifeCount;
	};

	std::vector<std::unique_ptr<particle>> particles;


	REFLECT_COMPONENT(ParticleComponent)
	{

	}


private:

};



#endif