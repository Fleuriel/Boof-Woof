#include "pch.h"
#include "AnimationComponent.h"
#include <iostream>
#include "ResourceManager/ResourceManager.h"


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

void AnimationComponent::Update(double deltaTime) {

    std::cout << "State Playing and AnimationaActive: " << state.isPlaying << '\t' << state.activeAnimation << '\n';

    if (!state.isPlaying) return;

    std::cout << "it entered this update in Animation\n";

    // Update animation state based on deltaTime
    //state.Update(deltaTime);

    

    UpdateAnim(deltaTime);

    // Apply bone transformations or other logic (e.g., interpolation between keyframes)
   //GetBoneTransformsAtTime(static_cast<float>(state.currentTime));  // This function would update vertex positions based on bones

    // Mark the mesh for an update, since the vertex positions might have changed
    updateMesh = true;
}

// Optional: Blend to a new animation
void AnimationComponent::BlendTo(const std::string& animationName, double blendDuration) {
    // Implementation of blending logic (if applicable)
    (void)blendDuration;
    (void)animationName;

    std::cerr << "BlendTo is not implemented yet." << std::endl;
}

// Get the current transformation matrix for a specific node (bone)
void AnimationComponent::GetCurrentTransform(const std::string& nodeName, aiMatrix4x4& outTransform) const {
    // Logic to calculate the transformation matrix for a specific node (e.g., skeletal bones)

    (void)nodeName;
    (void)outTransform;

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

void AnimationComponent::UpdateAnim(float deltaTime)
{
   // std::cout << "anim1\n" << '\n';
   // std::cout << g_ResourceManager.boneAnimations.size() << '\n';

	animation.UpdateAnimation(deltaTime);

  //  std::cout <<" ANIm2" << g_ResourceManager.boneAnimations.size() << '\n';
  //  std::cout << g_ResourceManager.boneAnimations.size() << '\n';
}