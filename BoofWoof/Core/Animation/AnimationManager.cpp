#include "pch.h"

#include "AnimationManager.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <iostream>
#include <cmath>


// Overload for subtraction
inline aiVector3D operator-(const aiVector3D& lhs, const aiVector3D& rhs) {
    return aiVector3D(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

// Overload for addition
inline aiVector3D operator+(const aiVector3D& lhs, const aiVector3D& rhs) {
    return aiVector3D(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

// Overload for scalar multiplication (vector * scalar)
inline aiVector3D operator*(const aiVector3D& vec, double scalar) {
    return aiVector3D(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

// Overload for scalar multiplication (scalar * vector)
inline aiVector3D operator*(double scalar, const aiVector3D& vec) {
    return aiVector3D(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

// Helper function for linear interpolation between two values
template <typename T>
T Lerp(const T& start, const T& end, double factor) {
    return start + static_cast<T>(factor * (end - start));
}




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

// Compute the transformation matrix for a given node
aiMatrix4x4 Animation::ComputeNodeTransformation(const std::string& nodeName, double currentTime) const {
    // Find the animation channel for the node
    const auto it = std::find_if(channels.begin(), channels.end(),
        [&nodeName](const AnimationChannel& channel) {
            return channel.nodeName == nodeName;
        });

    if (it == channels.end()) {
        // Node not found in animation, return identity matrix
        return aiMatrix4x4();
    }

    const AnimationChannel& channel = *it;

    aiVector3D interpolatedPosition(0.0f, 0.0f, 0.0f);
    aiQuaternion interpolatedRotation(1.0f, 0.0f, 0.0f, 0.0f);
    aiVector3D interpolatedScale(1.0f, 1.0f, 1.0f);

    // Interpolate position
    if (!channel.keyframes.empty()) {
        for (size_t i = 0; i < channel.keyframes.size() - 1; ++i) {
            const KeyFrame& current = channel.keyframes[i];
            const KeyFrame& next = channel.keyframes[i + 1];
            if (currentTime >= current.time && currentTime <= next.time) {
                double factor = (currentTime - current.time) / (next.time - current.time);
                interpolatedPosition = Lerp(current.position, next.position, factor);
                break;
            }
        }
    }

    // Interpolate rotation
    if (!channel.keyframes.empty()) {
        for (size_t i = 0; i < channel.keyframes.size() - 1; ++i) {
            const KeyFrame& current = channel.keyframes[i];
            const KeyFrame& next = channel.keyframes[i + 1];
            if (currentTime >= current.time && currentTime <= next.time) {
                double factor = (currentTime - current.time) / (next.time - current.time);
                aiQuaternion::Interpolate(interpolatedRotation, current.rotation, next.rotation, static_cast<float>(factor));
                interpolatedRotation.Normalize();
                break;
            }
        }
    }

    // Interpolate scale
    if (!channel.keyframes.empty()) {
        for (size_t i = 0; i < channel.keyframes.size() - 1; ++i) {
            const KeyFrame& current = channel.keyframes[i];
            const KeyFrame& next = channel.keyframes[i + 1];
            if (currentTime >= current.time && currentTime <= next.time) {
                double factor = (currentTime - current.time) / (next.time - current.time);
                interpolatedScale = Lerp(current.scale, next.scale, factor);
                break;
            }
        }
    }

    // Construct transformation matrix
    aiMatrix4x4 translationMatrix;
    aiMatrix4x4::Translation(interpolatedPosition, translationMatrix);

    aiMatrix4x4 rotationMatrix = aiMatrix4x4(interpolatedRotation.GetMatrix());

    aiMatrix4x4 scaleMatrix;
    aiMatrix4x4::Scaling(interpolatedScale, scaleMatrix);

    return translationMatrix * rotationMatrix * scaleMatrix;
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
        Animation animation(anim);

        animation.name = baseName + "_" + std::to_string(i);
        animations[animation.name] = animation;
        animationNames.push_back(animation.name);

        // Debug output
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
