#include "pch.h"
#include "AnimationComponent.h"
#include <iostream>

#define UNREFERENCED_PARAMETER(P) (P)

// Set an animation for the entity
void AnimationComponent::SetAnimation(const std::string& animationName) {
    state.activeAnimation = animationName;
    state.currentTime = 0.0; // Reset animation playback
}

// Play the current animation
void AnimationComponent::Play() {
    state.isPlaying = true;
}

// Stop the current animation
void AnimationComponent::Stop() {
    state.isPlaying = false;
}

// Update animation playback
void AnimationComponent::Update(double deltaTime) {
    if (!state.isPlaying || state.activeAnimation.empty()) return;

    state.Update(deltaTime);

    // Apply transformations (interpolation logic can go here if needed)
}

// Optional: Blend to a new animation
void AnimationComponent::BlendTo(const std::string& animationName, double blendDuration) {
    // Implementation of blending logic (if applicable)
    UNREFERENCED_PARAMETER(animationName);
    UNREFERENCED_PARAMETER(blendDuration);
    std::cerr << "BlendTo is not implemented yet." << std::endl;
}

// Get the current transformation matrix
void AnimationComponent::GetCurrentTransform(const std::string& nodeName, aiMatrix4x4& outTransform) const {
    // Logic to calculate the transformation matrix for a specific node (e.g., skeletal bones)
    UNREFERENCED_PARAMETER(nodeName);
    UNREFERENCED_PARAMETER(outTransform);
    std::cerr << "GetCurrentTransform is not implemented yet." << std::endl;
}

// Set an animation for a specific type
void AnimationComponent::SetAnimation(AnimationType type, const std::string& animationName) {
    animationMap[type] = animationName;
}

// Get the animation assigned to a specific type
std::string AnimationComponent::GetAnimation(AnimationType type) const {
    auto it = animationMap.find(type);
    if (it != animationMap.end()) {
        return it->second;
    }
    return ""; // Return an empty string if no animation is assigned
}
