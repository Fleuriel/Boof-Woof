#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "../assimp2016/assimp/scene.h"
#include "../assimp2016/assimp/Importer.hpp"
#include "Bones.h"
#include "Animation.h"





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

	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;

		if (!m_CurrentAnimation) return;

		float ticksPerSecond = m_CurrentAnimation->GetTicksPerSecond();
		float duration = m_CurrentAnimation->GetDuration();

		m_CurrentTime += ticksPerSecond * dt;

		// ✅ Check if using playback range
		if (m_UsingPlaybackRange)
		{
			// Loop within custom range
			if (m_CurrentTime > m_PlaybackEnd)
				m_CurrentTime = m_PlaybackStart;
		}
		else
		{
			// Loop over full animation
			m_CurrentTime = fmod(m_CurrentTime, duration);
			if (m_CurrentTime < 0.0f)
				m_CurrentTime += duration;
		}

		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}


	void SetPlaybackRange(float start, float end)
	{
		m_PlaybackStart = start;
		m_PlaybackEnd = end;
		m_CurrentTime = start; // reset to start of playback
		m_UsingPlaybackRange = true;
	}

	void SetAnimationTime(float dt) {
		m_CurrentTime = dt;
	}

	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		// Only apply scaling at the ROOT node (first call)
		if (node == &m_CurrentAnimation->GetRootNode())
		{
			parentTransform *= glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}


	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

	float GetCurrTime()
	{
		return m_CurrentTime;
	}

private:
	float m_PlaybackStart = 0.0f;
	float m_PlaybackEnd = std::numeric_limits<float>::max();
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
	bool m_UsingPlaybackRange = false;

};
