#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "../assimp2016/assimp/scene.h"
#include "../assimp2016/assimp/Importer.hpp"
#include "Bones.h"
#include "Animation.h"
#include "../Utilities/Components/AnimationComponent.hpp"



class AnimationComponent;

class Animator
{
public:
	Animator() {

	}

	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}


	void SetPlaybackRange(float start, float end);

	void UpdateAnimation(AnimationComponent& anim, float dt, float start = 0 , float end = 0);

	void SetAnimationTime(float dt);

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);


	std::vector<glm::mat4> GetFinalBoneMatrices();

	float GetCurrTime();

	bool m_UseCustomRange = false; // Flag to determine whether to loop within a range



private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

	float m_PlaybackStart = 0.0f;
	float m_PlaybackEnd = 0.0f;

};
