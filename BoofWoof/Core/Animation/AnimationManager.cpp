#include "pch.h"
#include "AnimationManager.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <cmath>

AnimationManager g_AnimationManager;

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
}

// Set animation type
void EntityAnimationState::SetAnimation(AnimationType type, const std::string& animationName) {
    animations[type] = animationName;
    if (activeAnimation.empty()) {
        activeAnimation = animationName; // Default to first assigned animation
    }
}

// Load Animations from a File
void AnimationManager::LoadAnimations(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "Error loading file: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Extract file name for naming
    std::string baseName = filePath.substr(filePath.find_last_of("/\\") + 1);
    baseName = baseName.substr(0, baseName.find_last_of('.'));

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        aiAnimation* anim = scene->mAnimations[i];
        double ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0;
        double duration = anim->mDuration;

        unsigned int segmentIndex = 0;
        for (double startTick = 0; startTick < duration; startTick += 70) { // 60-tick animations + 10-tick gap
            double endTick = std::min(startTick + 60, duration);

            Animation segmentAnimation;
            segmentAnimation.name = baseName + "_" + std::to_string(segmentIndex++);
            segmentAnimation.duration = endTick - startTick;
            segmentAnimation.ticksPerSecond = ticksPerSecond;

            for (unsigned int j = 0; j < anim->mNumChannels; ++j) {
                const aiNodeAnim* channel = anim->mChannels[j];
                AnimationChannel segmentChannel;
                segmentChannel.nodeName = channel->mNodeName.C_Str();

                for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k) {
                    if (channel->mPositionKeys[k].mTime >= startTick && channel->mPositionKeys[k].mTime <= endTick) {
                        KeyFrame keyframe;
                        keyframe.time = channel->mPositionKeys[k].mTime - startTick;
                        keyframe.position = channel->mPositionKeys[k].mValue;

                        if (k < channel->mNumRotationKeys) {
                            keyframe.rotation = channel->mRotationKeys[k].mValue;
                        }
                        if (k < channel->mNumScalingKeys) {
                            keyframe.scale = channel->mScalingKeys[k].mValue;
                        }
                        segmentChannel.keyframes.push_back(keyframe);
                    }
                }
                segmentAnimation.channels.push_back(segmentChannel);
            }

            animations[segmentAnimation.name] = segmentAnimation;

            animationNames.push_back(segmentAnimation.name);

            // Debug output
            std::cout << "Created animation segment: " << segmentAnimation.name << std::endl;
            std::cout << "  Start Tick: " << startTick << std::endl;
            std::cout << "  End Tick: " << endTick << std::endl;
            std::cout << "  Duration: " << segmentAnimation.duration << " ticks ("
                << segmentAnimation.duration / ticksPerSecond << " seconds)" << std::endl;
        }
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
void AnimationManager::AssignAnimation(const std::string& entityId, AnimationType type, const std::string& animationName) {
    if (animations.find(animationName) != animations.end()) {
        entityStates[entityId].SetAnimation(type, animationName);
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

// Update animations for all entities
void AnimationManager::Update(double deltaTime) {
    for (auto& [entityId, state] : entityStates) {
        if (state.isPlaying) {
            const auto& animation = animations[state.activeAnimation];
            state.Update(deltaTime, animation);

            // Apply interpolated transformation to the entity here
        }
    }
}

// Get the index of an animation by its name
int AnimationManager::GetAnimationIndex(const std::string& animationName) const {
    auto it = std::find(animationNames.begin(), animationNames.end(), animationName);
    if (it != animationNames.end()) {
        return static_cast<int>(std::distance(animationNames.begin(), it));
    }
    return -1; // Animation not found
}