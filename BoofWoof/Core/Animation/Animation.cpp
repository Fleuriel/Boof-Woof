// Animation.cpp

#include "pch.h"

#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

Animation::Animation() : duration(0.0f), ticksPerSecond(25.0f) {}

bool Animation::LoadFromScene(const aiScene* scene) {
    if (!scene->HasAnimations()) return false;

    const aiAnimation* aiAnim = scene->mAnimations[0];

    duration = static_cast<float>(aiAnim->mDuration);
    ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0f);

    for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i) {
        const aiNodeAnim* aiNode = aiAnim->mChannels[i];

        BoneAnimation boneAnim;
        boneAnim.boneName = aiNode->mNodeName.C_Str();

        for (unsigned int j = 0; j < aiNode->mNumPositionKeys; ++j) {
            KeyFrame keyFrame;
            keyFrame.timeStamp = static_cast<float>(aiNode->mPositionKeys[j].mTime);

            aiVector3D position = aiNode->mPositionKeys[j].mValue;
            keyFrame.position = glm::vec3(position.x, position.y, position.z);

            boneAnim.keyFrames.push_back(keyFrame);
        }

        for (unsigned int j = 0; j < aiNode->mNumRotationKeys; ++j) {
            float timeStamp = static_cast<float>(aiNode->mRotationKeys[j].mTime);
            glm::quat rotation = glm::quat_cast(glm::mat4(1.0f));

            if (j < boneAnim.keyFrames.size()) {
                boneAnim.keyFrames[j].rotation = rotation;
            }
            else {
                KeyFrame keyFrame;
                keyFrame.timeStamp = timeStamp;
                keyFrame.rotation = rotation;
                boneAnim.keyFrames.push_back(keyFrame);
            }
        }

        for (unsigned int j = 0; j < aiNode->mNumScalingKeys; ++j) {
            float timeStamp = static_cast<float>(aiNode->mScalingKeys[j].mTime);

            aiVector3D scale = aiNode->mScalingKeys[j].mValue;
            glm::vec3 scaling = glm::vec3(scale.x, scale.y, scale.z);

            if (j < boneAnim.keyFrames.size()) {
                boneAnim.keyFrames[j].scale = scaling;
            }
            else {
                KeyFrame keyFrame;
                keyFrame.timeStamp = timeStamp;
                keyFrame.scale = scaling;
                boneAnim.keyFrames.push_back(keyFrame);
            }
        }

        boneAnimations[boneAnim.boneName] = boneAnim;
    }

    return true;
}

bool Animation::LoadFromFile(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error: Assimp failed to load animation file: " << importer.GetErrorString() << std::endl;
        return false;
    }

    return LoadFromScene(scene);
}

float Animation::GetDuration() const {
    return duration;
}

float Animation::GetTicksPerSecond() const {
    return ticksPerSecond;
}

const std::vector<glm::mat4>& Animation::GetBoneTransformsAtTime(float currentTime) {
    float animationTime = fmod(currentTime * ticksPerSecond, duration);

    for (auto& [name, bone] : bones) {
        CalculateBoneTransform(animationTime, bone, glm::mat4(1.0f));
    }

    return boneTransforms;
}

void Animation::CalculateBoneTransform(float animationTime, Bone& bone, const glm::mat4& parentTransform) {
    glm::mat4 nodeTransform = glm::mat4(1.0f);

    if (boneAnimations.find(bone.name) != boneAnimations.end()) {
        const auto& boneAnim = boneAnimations[bone.name];

        // Interpolation logic (simplified for demo purposes)
        glm::vec3 interpolatedPosition = glm::vec3(0.0f);
        glm::quat interpolatedRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 interpolatedScale = glm::vec3(1.0f);

        nodeTransform = glm::translate(glm::mat4(1.0f), interpolatedPosition) *
            glm::toMat4(interpolatedRotation) *
            glm::scale(glm::mat4(1.0f), interpolatedScale);
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    if (bones.find(bone.name) != bones.end()) {
        size_t boneIndex = std::distance(bones.begin(), bones.find(bone.name));
        boneTransforms[boneIndex] = globalTransform * bone.offsetMatrix;
    }

    for (auto& childBone : bones) {
        CalculateBoneTransform(animationTime, childBone.second, globalTransform);
    }
}
