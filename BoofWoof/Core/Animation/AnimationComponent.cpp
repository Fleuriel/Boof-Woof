#include "pch.h"

#include "AnimationComponent.h"

AnimationComponent::AnimationComponent(std::shared_ptr<AnimationManager> animManager)
    : animationManager(std::move(animManager)) {}

void AnimationComponent::SetAnimation(const std::string& animationName) {
    if (animationManager->IsAnimationLoaded(animationName)) {
        state.activeAnimation = animationName;
        state.currentTime = 0.0;
    }
    else {
        std::cerr << "Animation not found: " << animationName << std::endl;
    }
}

void AnimationComponent::Play() {
    state.isPlaying = true;
}

void AnimationComponent::Stop() {
    state.isPlaying = false;
}

void AnimationComponent::Update(double deltaTime) {
    if (!state.isPlaying || state.activeAnimation.empty()) return;

    const Animation& animation = animationManager->GetAnimation(state.activeAnimation);
    state.Update(deltaTime, animation);

    // Apply transformations or interpolation logic here
}

void AnimationComponent::BlendTo(const std::string& animationName, double blendDuration) {
    // Implement blending logic (e.g., gradually transition from one animation to another)
}

void AnimationComponent::GetCurrentTransform(const std::string& nodeName, aiMatrix4x4& outTransform) const {
    // Calculate and return the interpolated transformation for the given node
}
