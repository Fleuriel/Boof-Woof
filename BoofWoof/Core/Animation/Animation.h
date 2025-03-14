#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "../assimp2016/assimp/scene.h"
#include "Bones.h"
#include <functional>
#include "AnimData.h"
#include "../../../BoofWoof/Core/Graphics/Model.h"

#include "AssimpHelper.h"


//class Animator;

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, Model* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate );


		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_Duration = static_cast<float>(animation->mDuration);
		m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;


		globalTransformation = globalTransformation.Inverse();


		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}



	~Animation()
	{
	}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}


	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		std::unordered_set<std::string> visitedNodes;
		assert(src);

		std::string nodeName = src->mName.C_Str();

		//  Detect Infinite Loop
		if (visitedNodes.find(nodeName) != visitedNodes.end())
		{
			std::cerr << "Warning: Circular reference detected in Assimp Node -> " << nodeName << std::endl;
			return; // Stop recursion
		}

		visitedNodes.insert(nodeName); // Mark as visited

		dest.name = nodeName;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		dest.children.reserve(src->mNumChildren);
		for (unsigned int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(std::move(newData));
		}

		visitedNodes.erase(nodeName); // Remove after processing
	}
	float m_Duration;
	float m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};