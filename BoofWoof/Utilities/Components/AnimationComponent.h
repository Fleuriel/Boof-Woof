#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../Core/Animation/Animation.h"

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
    double currentTime;          // Current time in the animation (in seconds)
    bool isPlaying;              // Playback status
};

class AnimationComponent {
public:
    AnimationComponent() = default;

    // Set the active animation
    void SetAnimation(const std::string& animationName);

    // Control playback
    void Play();
    void Stop();

    // Update the animation state
    void Update(double deltaTime);

    // Optional blending of animations
    void BlendTo(const std::string& animationName, double blendDuration);

    // Get the current transformation matrix for a specific node (bone)
    void GetCurrentTransform(const std::string& nodeName, aiMatrix4x4& outTransform) const;

    // Get the currently active animation name
    std::string GetActiveAnimation() const;

    // Get bone transformation matrices for the current animation time
    const std::vector<glm::mat4>& GetBoneTransformsAtTime(float currentTime);

    // Get the current time in ticks
    float GetTickCount() const;

private:
    EntityAnimationState state; // Entity-specific playback state
    Animation animation;        // The animation data
};

#endif // ANIMATION_COMPONENT_H