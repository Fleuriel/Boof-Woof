#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Bones.h"
#include "Animation.h"

#include <glm/gtx/string_cast.hpp >


class AnimationT;

class Animator
{
public:

	Animator(AnimationT* animation)
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
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

			std::cout << "Current Time: " << m_CurrentTime << std::endl;

			// Force identity to see if issue is with bone transforms
			for (int i = 0; i < m_FinalBoneMatrices.size(); i++)
			{
				m_FinalBoneMatrices[i] = glm::mat4(1.0f);
			}

			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(AnimationT* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* bone = m_CurrentAnimation->FindBone(nodeName);
		if (bone)
		{
			bone->Update(m_CurrentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;

			// Debugging
			std::cout << "Bone: " << nodeName << " (ID: " << index << ")\n";
			std::cout << "Offset Matrix:\n" << glm::to_string(offset) << "\n";
			std::cout << "Global Transformation:\n" << glm::to_string(globalTransformation) << "\n";

			m_FinalBoneMatrices[index] = m_CurrentAnimation->GetGlobalInverseTransform() * globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}
	}


	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	AnimationT* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

};