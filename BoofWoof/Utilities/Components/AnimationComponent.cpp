#include "pch.h"
#include "AnimationComponent.h"
#include <iostream>

// Set the active animation
void AnimationComponent::SetAnimation(const std::string& animationName) {
    state.activeAnimation = animationName;
    state.currentTime = 0.0; // Reset animation playback
}

// Start playing the animation
void AnimationComponent::Play() {
    state.isPlaying = true;
}

// Stop the animation
void AnimationComponent::Stop() {
    state.isPlaying = false;
}

// Update the animation state
void AnimationComponent::Update(double deltaTime) {
    if (!state.isPlaying || state.activeAnimation.empty()) return;

    state.Update(deltaTime);

    // Apply transformations (interpolation logic can go here if needed)
}

// Optional: Blend to a new animation
void AnimationComponent::BlendTo(const std::string& animationName, double blendDuration) {
    // Implementation of blending logic (if applicable)
    std::cerr << "BlendTo is not implemented yet." << std::endl;
}

// Get the current transformation matrix for a specific node (bone)
void AnimationComponent::GetCurrentTransform(const std::string& nodeName, aiMatrix4x4& outTransform) const {
    // Logic to calculate the transformation matrix for a specific node (e.g., skeletal bones)
    std::cerr << "GetCurrentTransform is not implemented yet." << std::endl;
}

// Get the currently active animation name
std::string AnimationComponent::GetActiveAnimation() const {
    return state.activeAnimation;
}

// Get bone transformation matrices for the current animation time
const std::vector<glm::mat4>& AnimationComponent::GetBoneTransformsAtTime(float currentTime) {
    return animation.GetBoneTransformsAtTime(currentTime);
}

// Get the current time in ticks
float AnimationComponent::GetTickCount() const {
    return static_cast<float>(state.currentTime);
}