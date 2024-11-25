#include "pch.h"
#include "AnimationComponent.h"

const float AnimationComponent::ANIMATION_BLEND_TIME = 0.3f;

AnimationComponent::AnimationComponent()
    : m_currentClip(nullptr)
    , m_nextClip(nullptr)
    , m_currentState(AnimationState::IDLE)
    , m_nextState(AnimationState::IDLE)
    , m_fadeTime(0.0f)
    , m_currentFadeTime(0.0f)
    , m_currentTime(0.0f)
    , m_playbackSpeed(1.0f)
    , m_isPlaying(false)
    , m_isLooping(true)
    , m_boneCount(0) {
    m_finalBoneMatrices.resize(MAX_BONES);
    for (auto& matrix : m_finalBoneMatrices) {
        matrix = glm::mat4(1.0f);
    }
}

AnimationComponent::~AnimationComponent() {
    m_animations.clear();
}

void AnimationComponent::update(float deltaTime) {
    if (!m_isPlaying || !m_currentClip) return;

    // Update current time
    m_currentTime += deltaTime * m_playbackSpeed;

    // Handle looping
    if (m_currentTime >= m_currentClip->duration) {
        if (m_isLooping) {
            m_currentTime = fmod(m_currentTime, m_currentClip->duration);
        }
        else {
            m_currentTime = m_currentClip->duration;
            m_isPlaying = false;
            return;
        }
    }

    // Handle crossfading
    if (m_nextClip) {
        m_currentFadeTime += deltaTime;
        if (m_currentFadeTime >= m_fadeTime) {
            // Transition complete
            m_currentClip = m_nextClip;
            m_currentState = m_nextState;
            m_nextClip = nullptr;
            m_currentTime = 0.0f;
            m_currentFadeTime = 0.0f;
        }
    }

    // Calculate bone transforms
    calculateBoneTransform("root", glm::mat4(1.0f));
}

void AnimationComponent::playState(AnimationState state, bool loop) {
    auto it = m_animations.find(state);
    if (it == m_animations.end()) return;

    m_currentClip = it->second;
    m_currentState = state;
    m_nextClip = nullptr;
    m_currentTime = 0.0f;
    m_isPlaying = true;
    m_isLooping = loop;
}

void AnimationComponent::stop() {
    m_isPlaying = false;
    m_currentTime = 0.0f;
    m_nextClip = nullptr;
    m_currentFadeTime = 0.0f;
}

void AnimationComponent::pause() {
    m_isPlaying = false;
}

void AnimationComponent::resume() {
    m_isPlaying = true;
}

void AnimationComponent::setTime(float time) {
    m_currentTime = std::clamp(time, 0.0f, m_currentClip ? m_currentClip->duration : 0.0f);
}

void AnimationComponent::crossFadeTo(AnimationState state, float fadeTime) {
    auto it = m_animations.find(state);
    if (it == m_animations.end() || !m_currentClip) return;

    m_nextClip = it->second;
    m_nextState = state;
    m_fadeTime = fadeTime;
    m_currentFadeTime = 0.0f;
    m_isPlaying = true;
}

bool AnimationComponent::hasAnimation(AnimationState state) const {
    return m_animations.find(state) != m_animations.end();
}

int AnimationComponent::getBoneId(const std::string& boneName) const {
    auto it = m_boneMapping.find(boneName);
    return it != m_boneMapping.end() ? it->second : -1;
}

const glm::mat4& AnimationComponent::getBoneTransform(int boneId) const {
    if (boneId >= 0 && boneId < m_finalBoneMatrices.size()) {
        return m_finalBoneMatrices[boneId];
    }
    static const glm::mat4 identityMatrix(1.0f);
    return identityMatrix;
}

const glm::mat4& AnimationComponent::getBoneTransform(const std::string& boneName) const {
    return getBoneTransform(getBoneId(boneName));
}

void AnimationComponent::calculateBoneTransform(const std::string& nodeName, const glm::mat4& parentTransform) {
    // Start with identity matrix for node transformation
    glm::mat4 nodeTransform(1.0f);

    // Early exit if no current animation clip
    if (!m_currentClip) {
        return;
    }

    // Find bone info for current node
    auto currentBoneIt = m_currentClip->bones.find(nodeName);
    BoneInfo* currentBone = (currentBoneIt != m_currentClip->bones.end()) ? &currentBoneIt->second : nullptr;

    if (currentBone && !currentBone->keyFrames.empty()) {
        // Ensure m_currentTime is within animation bounds
        float normalizedTime = fmod(m_currentTime, currentBone->keyFrames.back().timeStamp);
        if (normalizedTime < 0.0f) normalizedTime += currentBone->keyFrames.back().timeStamp;

        // Calculate the interpolated keyframe
        KeyFrame interpolatedFrame = interpolateKeyFrames(normalizedTime, currentBone->keyFrames);

        // Build transformation matrix
        nodeTransform = calculateTransformMatrix(interpolatedFrame);

        // Handle crossfading between animations
        if (m_nextClip && m_fadeTime > 0.0f) {
            auto nextBoneIt = m_nextClip->bones.find(nodeName);
            if (nextBoneIt != m_nextClip->bones.end()) {
                const BoneInfo& nextBone = nextBoneIt->second;
                if (!nextBone.keyFrames.empty()) {
                    // Calculate blend factor, clamped between 0 and 1
                    float blendFactor = glm::clamp(m_currentFadeTime / m_fadeTime, 0.0f, 1.0f);

                    // Get the next animation's transform
                    KeyFrame nextFrame = interpolateKeyFrames(0.0f, nextBone.keyFrames);
                    glm::mat4 nextTransform = calculateTransformMatrix(nextFrame);

                    // Interpolate between current and next transform
                    nodeTransform = interpolateMatrix(nodeTransform, nextTransform, blendFactor);
                }
            }
        }
    }

    // Calculate global transform
    glm::mat4 globalTransform = parentTransform * nodeTransform;

    // Apply final transform to bone if it exists in the skeleton
    if (currentBone) {
        int boneIndex = getBoneId(nodeName);
        if (boneIndex >= 0 && boneIndex < static_cast<int>(m_finalBoneMatrices.size())) {
            m_finalBoneMatrices[boneIndex] = globalTransform * currentBone->offset;
        }
    }

    // Process child nodes
    if (m_currentClip->rootNode) {
        for (unsigned int i = 0; i < m_currentClip->rootNode->mNumChildren; i++) {
            const aiNode* childNode = m_currentClip->rootNode->mChildren[i];
            if (childNode && childNode->mName.length > 0) {
                calculateBoneTransform(childNode->mName.C_Str(), globalTransform);
            }
        }
    }
}
KeyFrame AnimationComponent::interpolateKeyFrames(float animationTime, const std::vector<KeyFrame>& keyFrames) {
    if (keyFrames.empty()) return KeyFrame();
    if (keyFrames.size() == 1) return keyFrames[0];

    // Find the two keyframes to interpolate between
    size_t nextFrameIndex = 0;
    while (nextFrameIndex < keyFrames.size() && keyFrames[nextFrameIndex].timeStamp <= animationTime) {
        nextFrameIndex++;
    }

    if (nextFrameIndex == 0) return keyFrames[0];
    if (nextFrameIndex == keyFrames.size()) return keyFrames.back();

    const KeyFrame& currentFrame = keyFrames[nextFrameIndex - 1];
    const KeyFrame& nextFrame = keyFrames[nextFrameIndex];

    float deltaTime = nextFrame.timeStamp - currentFrame.timeStamp;
    float factor = (animationTime - currentFrame.timeStamp) / deltaTime;

    KeyFrame result;
    result.timeStamp = animationTime;
    result.position = glm::mix(currentFrame.position, nextFrame.position, factor);
    result.rotation = glm::slerp(currentFrame.rotation, nextFrame.rotation, factor);
    result.scale = glm::mix(currentFrame.scale, nextFrame.scale, factor);

    return result;
}

const std::string& AnimationComponent::getCurrentClipName() const {
    // Ensure the current clip is not null
    if (m_currentClip) {
        return m_currentClip->name;
    }
    static const std::string emptyString = "";
    return emptyString;  // Return empty string if there's no current clip
}
