#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <string>
#include <memory>
#include <unordered_map>
#include "../Core/Animation/AnimationManager.h"


class AnimationComponent {
public:
    AnimationComponent() = default;

    explicit AnimationComponent(std::shared_ptr<AnimationManager> animManager);

    void SetAnimation(const std::string& animationName);
    void Play();
    void Stop();
    void Update(double deltaTime);

    // Optional blending of animations
    void BlendTo(const std::string& animationName, double blendDuration);

    // Get the current transformation matrix
    void GetCurrentTransform(const std::string& nodeName, aiMatrix4x4& outTransform) const;

    // Set and Get animation by type
    void SetAnimation(AnimationType type, const std::string& animationName);
    std::string GetAnimation(AnimationType type) const;

    // Clear a specific animation type
    void ClearAnimation(AnimationType type);

    // Clear all animations
    void ClearAllAnimations();

private:
    std::shared_ptr<AnimationManager> animationManager; // Shared animation manager
    EntityAnimationState state;                         // Entity-specific playback state
    std::string entityId;                               // Unique identifier for the entity

    // Map to store animations by type
    std::unordered_map<AnimationType, std::string> animationMap;
};

#endif // ANIMATION_COMPONENT_H
