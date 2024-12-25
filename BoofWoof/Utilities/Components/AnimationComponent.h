#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <string>
#include <unordered_map>
#include "../Core/Animation/Animation.h"

enum AnimationType {
    Idle = 0,
    Moving,
    Action
};

class EntityAnimationState {
public:
    EntityAnimationState()
        : currentTime(0.0), isPlaying(false) {}

    void Update(double deltaTime) {
        if (isPlaying) {
            currentTime += deltaTime;
        }
    }

    std::string activeAnimation; // The currently active animation
    double currentTime;          // Current time in the animation
    bool isPlaying;              // Playback status
};

class AnimationComponent {
public:
    AnimationComponent() = default;

    // Set an animation for the component
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

private:
    EntityAnimationState state;                         // Entity-specific playback state

    // Map to store animations by type
    std::unordered_map<AnimationType, std::string> animationMap;
};

#endif // ANIMATION_COMPONENT_H
