// EntityAnimator.h
#ifndef ENTITYANIMATOR_H
#define ENTITYANIMATOR_H


#include "../Utilities/Components/AnimationComponent.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


class EntityAnimator {
public:
    EntityAnimator(const std::string& entityId) : m_entityId(entityId) {}

    // Animation state for a specific clip on this entity
    struct AnimationState {
        bool isPlaying = false;
        bool isLooping = false;
        float currentTime = 0.0f;
        float playbackSpeed = 1.0f;
        float weight = 1.0f;  // For blending multiple animations
        AnimationClip* clip = nullptr;
    };

    // Add an animation to this entity's available set
    void addAnimation(const std::string& name, AnimationClip* clip) {
        if (clip && m_animations.find(name) == m_animations.end()) {
            m_animations[name] = clip;
            m_animationStates[name] = AnimationState();
            m_animationStates[name].clip = clip;
        }
    }

    // Remove an animation from this entity
    void removeAnimation(const std::string& name) {
        m_animations.erase(name);
        m_animationStates.erase(name);
    }

    // Play a specific animation
    void playAnimation(const std::string& name, bool loop = true) {
        auto stateIt = m_animationStates.find(name);
        if (stateIt != m_animationStates.end()) {
            stateIt->second.isPlaying = true;
            stateIt->second.isLooping = loop;
            stateIt->second.currentTime = 0.0f;
            stateIt->second.weight = 1.0f;
            
            // Optional: Stop other animations if not blending
            for (auto& [otherName, state] : m_animationStates) {
                if (otherName != name) {
                    state.isPlaying = false;
                    state.weight = 0.0f;
                }
            }
        }
    }

    // Play multiple animations with blending
    void blendAnimations(const std::vector<std::pair<std::string, float>>& animationWeights) {
        float totalWeight = 0.0f;
        
        // First pass: calculate total weight and validate
        for (const auto& [name, weight] : animationWeights) {
            if (m_animationStates.find(name) != m_animationStates.end()) {
                totalWeight += weight;
            }
        }

        if (totalWeight > 0.0f) {
            // Second pass: normalize weights and set states
            for (const auto& [name, weight] : animationWeights) {
                auto stateIt = m_animationStates.find(name);
                if (stateIt != m_animationStates.end()) {
                    stateIt->second.isPlaying = true;
                    stateIt->second.weight = weight / totalWeight;
                }
            }
        }
    }

    // Update animation states and calculate final pose
    void update(float deltaTime) {
        // Update all active animations
        for (auto& [name, state] : m_animationStates) {
            if (state.isPlaying) {
                updateAnimationState(state, deltaTime);
            }
        }

        // Calculate final bone transforms
        calculateFinalPose();
    }

    // Get the final bone transforms for rendering
    const std::vector<glm::mat4>& getFinalBoneMatrices() const {
        return m_finalBoneMatrices;
    }

    const std::unordered_map<std::string, AnimationState>& getAnimationStates() const {
        return m_animationStates;
    }

    void removeAllAnimations() {
        m_animations.clear();
        m_animationStates.clear();
    }

private:
    KeyFrame interpolateKeyFrames(float animationTime, const std::vector<KeyFrame>& keyFrames) {
        if (keyFrames.empty()) {
            return KeyFrame(); // Return a default keyframe if no keyframes exist
        }

        // If there's only one keyframe, return it
        if (keyFrames.size() == 1) {
            return keyFrames[0];
        }

        KeyFrame result;

        // Find the two surrounding keyframes
        KeyFrame keyFrame1, keyFrame2;
        for (size_t i = 0; i < keyFrames.size() - 1; ++i) {
            if (animationTime >= keyFrames[i].timeStamp && animationTime < keyFrames[i + 1].timeStamp) {
                keyFrame1 = keyFrames[i];
                keyFrame2 = keyFrames[i + 1];
                break;
            }
        }

        // Interpolation factor (between 0 and 1)
        float deltaTime = keyFrame2.timeStamp - keyFrame1.timeStamp;
        float factor = (animationTime - keyFrame1.timeStamp) / deltaTime;

        // Interpolate position
        result.position = glm::mix(keyFrame1.position, keyFrame2.position, factor);

        // Interpolate rotation
        result.rotation = glm::slerp(keyFrame1.rotation, keyFrame2.rotation, factor);

        // Interpolate scale
        result.scale = glm::mix(keyFrame1.scale, keyFrame2.scale, factor);

        return result;
    }

    void updateAnimationState(AnimationState& state, float deltaTime) {
        if (!state.clip) return;

        state.currentTime += deltaTime * state.playbackSpeed;
        
        if (state.currentTime >= state.clip->duration) {
            if (state.isLooping) {
                state.currentTime = fmod(state.currentTime, state.clip->duration);
            } else {
                state.currentTime = state.clip->duration;
                state.isPlaying = false;
            }
        }
    }

    void calculateFinalPose() {
        // Reset final matrices
        m_finalBoneMatrices.clear();
        m_finalBoneMatrices.resize(MAX_BONES, glm::mat4(1.0f));

        std::unordered_map<std::string, glm::mat4> blendedTransforms;

        // Calculate transforms for each active animation
        for (const auto& [name, state] : m_animationStates) {
            if (state.isPlaying && state.weight > 0.0f && state.clip) {
                calculateAnimationPose(state, blendedTransforms);
            }
        }

        // Apply final blended transforms to bones
        for (const auto& [boneName, transform] : blendedTransforms) {
            auto boneId = getBoneId(boneName);
            if (boneId >= 0 && boneId < MAX_BONES) {
                m_finalBoneMatrices[boneId] = transform;
            }
        }
    }

    void calculateAnimationPose(const AnimationState& state, 
                              std::unordered_map<std::string, glm::mat4>& blendedTransforms) {
        for (const auto& [boneName, boneInfo] : state.clip->bones) {
            // Get interpolated keyframe for current time
            KeyFrame frame = interpolateKeyFrames(state.currentTime, boneInfo.keyFrames);

            // Calculate bone transform
            glm::mat4 transform = 
                glm::translate(glm::mat4(1.0f), frame.position) *
                glm::toMat4(frame.rotation) *
                glm::scale(glm::mat4(1.0f), frame.scale);

            // Blend with existing transform
            auto& finalTransform = blendedTransforms[boneName];
            if (finalTransform == glm::mat4(1.0f)) {
                finalTransform = transform * state.weight;
            } else {
                finalTransform += transform * state.weight;
            }
        }
    }

    // Helper function to get bone ID (implement based on your bone mapping system)
    int getBoneId(const std::string& boneName) const {
        auto it = m_boneMapping.find(boneName);
        return it != m_boneMapping.end() ? it->second : -1;
    }

    static const int MAX_BONES = 100;
    std::string m_entityId;
    std::unordered_map<std::string, AnimationClip*> m_animations;
    std::unordered_map<std::string, AnimationState> m_animationStates;
    std::unordered_map<std::string, int> m_boneMapping;
    std::vector<glm::mat4> m_finalBoneMatrices;
};

#endif // !1