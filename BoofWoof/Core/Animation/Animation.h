#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "Bones.h"
#include <functional>
#include "../../../BoofWoof/Core/Graphics/Model.h"
#include "../../../BoofWoof/Core/Graphics/Mesh.h"



class Animator;

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class AnimationT
{
public:
	AnimationT() = default;



	AnimationT(const std::string& animationPath, Model* model)
	{
		auto ConvertMatrixToGLM = [](const aiMatrix4x4& from) -> glm::mat4 {
			return glm::mat4{
				{ from.a1, from.b1, from.c1, from.d1 },
				{ from.a2, from.b2, from.c2, from.d2 },
				{ from.a3, from.b3, from.c3, from.d3 },
				{ from.a4, from.b4, from.c4, from.d4 }
			};
		};

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;

		m_GlobalInverseTransform = ConvertMatrixToGLM(scene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);

		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;

		globalTransformation = globalTransformation.Inverse();
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}

	~AnimationT()
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

	glm::mat4 GetGlobalInverseTransform() const {
		return m_GlobalInverseTransform;
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
		assert(src);

		auto ConvertMatrixToGLM = [](const aiMatrix4x4& from) -> glm::mat4 {
			return glm::mat4{
				{ from.a1, from.b1, from.c1, from.d1 },
				{ from.a2, from.b2, from.c2, from.d2 },
				{ from.a3, from.b3, from.c3, from.d3 },
				{ from.a4, from.b4, from.c4, from.d4 }
			};
			};

		dest.name = src->mName.data;
		dest.transformation = ConvertMatrixToGLM(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;

	glm::mat4 m_GlobalInverseTransform;
};
