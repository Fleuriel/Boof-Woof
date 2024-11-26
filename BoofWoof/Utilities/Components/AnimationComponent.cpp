#include "pch.h"

#include "AnimationComponent.h"
#include <iostream>

#define UNREFERENCED_PARAMETER(P)          (P)


// Constructor
AnimationComponent::AnimationComponent(std::shared_ptr<AnimationManager> animManager)
    : animationManager(std::move(animManager)) {}

// Set an animation for the entity
void AnimationComponent::SetAnimation(const std::string& animationName) {
    if (!animationManager->IsAnimationLoaded(animationName)) {
        std::cerr << "Animation not found: " << animationName << std::endl;
        return;
    }
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
    if (!state.isPlaying) return;

    const auto& animation = animationManager->GetAnimation(state.activeAnimation);
    state.Update(deltaTime, animation);

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
    if (!animationManager->IsAnimationLoaded(animationName)) {
        std::cerr << "Animation not found: " << animationName << std::endl;
        return;
    }
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

// Clear a specific animation type
void AnimationComponent::ClearAnimation(AnimationType type) {
    auto it = animationMap.find(type);
    if (it != animationMap.end()) {
        animationMap.erase(it);
        std::cout << "Cleared animation for type: " << static_cast<int>(type) << std::endl;
    }
    else {
        std::cerr << "No animation found for type: " << static_cast<int>(type) << std::endl;
    }
}

// Clear all animations
void AnimationComponent::ClearAllAnimations() {
    animationMap.clear();
    state.activeAnimation.clear();
    state.isPlaying = false;
    state.currentTime = 0.0;
    std::cout << "All animations cleared." << std::endl;
}
