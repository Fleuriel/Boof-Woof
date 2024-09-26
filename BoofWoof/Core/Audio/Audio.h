#pragma once
#include "../ECS/System.hpp"
#include <fmod.hpp>
#include <fmod_errors.h>

class Audio : public System
{
public: 
	void Init();
	void Update(float deltaTime);

private:
	FMOD::System* system;
	FMOD_RESULT result;
	FMOD::Sound* sound;
	FMOD::Channel* channel;
};