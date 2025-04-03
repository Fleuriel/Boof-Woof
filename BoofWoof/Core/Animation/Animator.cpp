
#include "pch.h"
#include "Animator.h"
#include "../Utilities/Components/AnimationComponent.hpp"


void Animator::UpdateAnimation(AnimationComponent& anim, float dt)
{
	if (!anim.pauseAnimation)
	{
		m_DeltaTime = dt;
		//std::cout <<"Animator.h : \t" << m_DeltaTime << '\n';

		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}
}

void Animator::SetAnimationTime(float dt) {
	m_CurrentTime = dt;
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
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


std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}

float Animator::GetCurrTime()
{
	return m_CurrentTime;
}