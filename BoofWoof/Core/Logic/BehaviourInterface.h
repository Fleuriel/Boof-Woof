#pragma once
#ifndef BEHAVIOURINTERFACE_H
#define BEHAVIOURINTERFACE_H

#include "../ECS/pch.h"


struct Behaviour_i
{
	virtual ~Behaviour_i() = default;
	virtual void Init(Entity entity) = 0;
	virtual void Update(Entity entity) = 0;
	virtual void Destroy(Entity entity) = 0;
	virtual const char* getBehaviourName() = 0;
};


struct input_interface
{
	virtual bool isButtonPressed(std::uint32_t Key) = 0;
};

struct engine_interface
{
	virtual ~engine_interface() = default;
	virtual input_interface& getInputSystem() = 0;

	virtual bool HaveTransformComponent(Entity entity) = 0;
	virtual glm::vec3 GetPosition(Entity entity) = 0;
	virtual void SetPosition(Entity entity, glm::vec3 position) = 0;
	
	virtual bool HaveVelocityComponent(Entity entity) = 0;
	virtual void SetVelocity(Entity entity, glm::vec3 velocity) = 0;
	//virtual void ChangePosition(Entity entity, float x, float y, float z) = 0;
};

using GetScripts_cpp_t = std::vector<std::unique_ptr<Behaviour_i>>* (*)(engine_interface* pEI);

#endif // !BEHAVIOURINTERFACE_H