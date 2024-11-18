#include "pch.h"

#include "AnimationManager.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <cmath>

// Animation Constructor
Animation::Animation(const aiAnimation* assimpAnimation) {
    name = assimpAnimation->mName.C_Str();
    duration = assimpAnimation->mDuration;
    ticksPerSecond = assimpAnimation->mTicksPerSecond != 0 ? assimpAnimation->mTicksPerSecond : 25.0;

    for (unsigned int i = 0; i < assimpAnimation->mNumChannels; ++i) {
        const aiNodeAnim* channel = assimpAnimation->mChannels[i];
        AnimationChannel animationChannel;
        animationChannel.nodeName = channel->mNodeName.C_Str();

        for (unsigned int j = 0; j < channel->mNumPositionKeys; ++j) {
            KeyFrame keyframe;
            keyframe.time = channel->mPositionKeys[j].mTime;
            keyframe.position = channel->mPositionKeys[j].mValue;

            if (j < channel->mNumRotationKeys) {
                keyframe.rotation = channel->mRotationKeys[j].mValue;
            }
            if (j < channel->mNumScalingKeys) {
                keyframe.scale = channel->mScalingKeys[j].mValue;
            }

            animationChannel.keyframes.push_back(keyframe);
        }
        channels.push_back(animationChannel);
    }
}

// EntityAnimationState Update Function
void EntityAnimationState::Update(double deltaTime, const Animation& animation) {
    if (!isPlaying || activeAnimation.empty()) return;

    currentTime += deltaTime * animation.ticksPerSecond;
    if (currentTime > animation.duration) {
        currentTime = fmod(currentTime, animation.duration); // Loop animation
    }

    // Add logic here to apply interpolated transformations to the entity
}

// AnimationManager::LoadAnimations
void AnimationManager::LoadAnimations(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "Error loading file: " << importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        Animation animation(scene->mAnimations[i]);
        animations[animation.name] = animation;
        std::cout << "Loaded animation: " << animation.name << std::endl;
    }
}

// Check if an animation is loaded
bool AnimationManager::IsAnimationLoaded(const std::string& animationName) const {
    return animations.find(animationName) != animations.end();
}

// Retrieve an animation by name
const Animation& AnimationManager::GetAnimation(const std::string& animationName) const {
    auto it = animations.find(animationName);
    if (it == animations.end()) {
        throw std::runtime_error("Animation not found: " + animationName);
    }
    return it->second;
}

// Assign an animation to an entity
void AnimationManager::AssignAnimation(const std::string& entityId, const std::string& animationName) {
    if (animations.find(animationName) != animations.end()) {
        entityStates[entityId].activeAnimation = animationName;
        entityStates[entityId].currentTime = 0.0;
    }
    else {
        std::cerr << "Animation not found: " << animationName << std::endl;
    }
}

// Play animation for an entity
void AnimationManager::PlayAnimation(const std::string& entityId) {
    if (entityStates.find(entityId) != entityStates.end()) {
        entityStates[entityId].isPlaying = true;
    }
}

// Stop animation for an entity
void AnimationManager::StopAnimation(const std::string& entityId) {
    if (entityStates.find(entityId) != entityStates.end()) {
        entityStates[entityId].isPlaying = false;
    }
}

// Update all entity animations
void AnimationManager::Update(double deltaTime) {
    for (auto& [entityId, state] : entityStates) {
        if (state.isPlaying) {
            const auto& animation = animations[state.activeAnimation];
            state.Update(deltaTime, animation);

            // Apply interpolated transformation to the entity here
            // (e.g., Fetch keyframes from `animation` and interpolate)
        }
    }
}
