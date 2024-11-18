#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <string>
#include <memory>
#include "../Core/Animation/AnimationManager.h"

class AnimationComponent {
public:
  //  AnimationComponent();
  // ~AnimationComponent();

    AnimationComponent() = default;


    AnimationComponent(std::shared_ptr<AnimationManager> animManager);

    void SetAnimation(const std::string& animationName);
    void Play();
    void Stop();
    void Update(double deltaTime);

    // Optionally, blend animations
    void BlendTo(const std::string& animationName, double blendDuration);

    // Get the current transformation matrix
    void GetCurrentTransform(const std::string& nodeName, aiMatrix4x4& outTransform) const;

private:
    std::shared_ptr<AnimationManager> animationManager; // Shared animation manager
    EntityAnimationState state;                         // Entity-specific playback state
    std::string entityId;                               // Unique identifier for the entity
};

#endif // ANIMATION_COMPONENT_H
