// AnimationComponent.cpp
#include "pch.h"
#include "AnimationComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

const float AnimationComponent::ANIMATION_BLEND_TIME = 0.25f;

AnimationComponent::AnimationComponent()
    : m_currentClip(nullptr)
    , m_nextClip(nullptr)
    , m_fadeTime(0.0f)
    , m_currentFadeTime(0.0f)
    , m_currentTime(0.0f)
    , m_playbackSpeed(1.0f)
    , m_isPlaying(false)
    , m_isLooping(false)
    , m_boneCount(0)
{
    m_finalBoneMatrices.resize(MAX_BONES);
    for (auto& matrix : m_finalBoneMatrices) {
        matrix = glm::mat4(1.0f);
    }
}

AnimationComponent::~AnimationComponent() {
    m_animations.clear();
}

void AnimationComponent::loadAnimation(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_LimitBoneWeights |
        aiProcess_GlobalScale |
        aiProcess_PopulateArmatureData
    );

    if (!scene || !scene->HasAnimations()) {
        std::cerr << "Failed to load animation file: " << filename << "\n";
        std::cerr << "Assimp error: " << importer.GetErrorString() << "\n";
        return;
    }

    loadAnimation(scene, std::filesystem::path(filename).stem().string());
}

void AnimationComponent::processAnimation(const aiNodeAnim* nodeAnim, AnimationClip* clip) {
    std::string nodeName = nodeAnim->mNodeName.C_Str();
    BoneInfo& boneInfo = clip->bones[nodeName];
    boneInfo.name = nodeName;

    // Process position keyframes
    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys; i++) {
        KeyFrame keyFrame;
        aiVectorKey posKey = nodeAnim->mPositionKeys[i];
        keyFrame.timeStamp = static_cast<float>(posKey.mTime);
        keyFrame.position = glm::vec3(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z);

        // Find corresponding rotation key
        if (i < nodeAnim->mNumRotationKeys) {
            aiQuatKey rotKey = nodeAnim->mRotationKeys[i];
            keyFrame.rotation = glm::quat(rotKey.mValue.w, rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z);
        }

        // Find corresponding scale key
        if (i < nodeAnim->mNumScalingKeys) {
            aiVectorKey scaleKey = nodeAnim->mScalingKeys[i];
            keyFrame.scale = glm::vec3(scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z);
        }

        boneInfo.keyFrames.push_back(keyFrame);
    }
}

void AnimationComponent::processNode(aiNode* node, const glm::mat4& parentTransform, AnimationClip* clip) {
    std::string nodeName = node->mName.C_Str();

    // Convert aiMatrix4x4 to glm::mat4
    glm::mat4 nodeTransform;
    memcpy(&nodeTransform, &node->mTransformation, sizeof(float) * 16);

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    // If this node corresponds to a bone in our animation
    if (clip->bones.find(nodeName) != clip->bones.end()) {
        BoneInfo& boneInfo = clip->bones[nodeName];
        boneInfo.offset = globalTransform;

        // Assign bone ID if not already assigned
        if (m_boneMapping.find(nodeName) == m_boneMapping.end()) {
            m_boneMapping[nodeName] = m_boneCount++;
        }
        boneInfo.id = m_boneMapping[nodeName];
    }

    // Process all child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], globalTransform, clip);
    }
}

void AnimationComponent::loadAnimation(const aiScene* scene, const std::string& animationName) {
    if (!scene->HasAnimations()) return;

    for (unsigned int animIndex = 0; animIndex < scene->mNumAnimations; animIndex++) {
        aiAnimation* animation = scene->mAnimations[animIndex];
        std::string clipName = animationName;

        if (scene->mNumAnimations > 1) {
            clipName += "_" + std::to_string(animIndex);
        }

        auto clip = std::make_unique<AnimationClip>();
        clip->name = clipName;
        clip->duration = static_cast<float>(animation->mDuration);
        clip->ticksPerSecond = animation->mTicksPerSecond != 0 ?
            static_cast<float>(animation->mTicksPerSecond) : 24.0f;

        clip->rootNode = scene->mRootNode;

        // Process all animation channels (bone animations)
        for (unsigned int channelIndex = 0; channelIndex < animation->mNumChannels; channelIndex++) {
            aiNodeAnim* channel = animation->mChannels[channelIndex];
            processAnimation(channel, clip.get());
        }

        // Process the bone hierarchy
        processNode(scene->mRootNode, glm::mat4(1.0f), clip.get());

        m_animations[clipName] = std::move(clip);
    }
}


void AnimationComponent::update(float deltaTime) {
    if (!m_isPlaying || !m_currentClip) return;

    // Update animation time
    m_currentTime += deltaTime * m_playbackSpeed * m_currentClip->ticksPerSecond;

    // Handle animation looping
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

    // Handle animation blending if there's a next clip
    if (m_nextClip) {
        m_currentFadeTime += deltaTime;
        if (m_currentFadeTime >= m_fadeTime) {
            m_currentClip = m_nextClip;
            m_nextClip = nullptr;
            m_currentFadeTime = 0.0f;
            m_currentTime = 0.0f;
        }
    }

    // Calculate bone transformations
    calculateBoneTransform("root", glm::mat4(1.0f));
}

void AnimationComponent::calculateBoneTransform(const std::string& nodeName, const glm::mat4& parentTransform) {
    glm::mat4 nodeTransform = glm::mat4(1.0f);

    // Get current animation transform
    auto currentBoneIt = m_currentClip->bones.find(nodeName);
    if (currentBoneIt != m_currentClip->bones.end()) {
        const BoneInfo& boneInfo = currentBoneIt->second;

        // Get interpolated keyframe
        KeyFrame currentFrame = interpolateKeyFrames(m_currentTime, boneInfo.keyFrames);

        // Create transformation matrix
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), currentFrame.position);
        glm::mat4 rotation = glm::toMat4(currentFrame.rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), currentFrame.scale);

        nodeTransform = translation * rotation * scale;

        // Handle blending with next animation if it exists
        if (m_nextClip) {
            auto nextBoneIt = m_nextClip->bones.find(nodeName);
            if (nextBoneIt != m_nextClip->bones.end()) {
                const BoneInfo& nextBoneInfo = nextBoneIt->second;
                KeyFrame nextFrame = interpolateKeyFrames(0.0f, nextBoneInfo.keyFrames); // Interpolate from the start of the next animation

                float blendFactor = m_currentFadeTime / m_fadeTime;

                // Interpolate between current and next animation
                glm::vec3 blendedPosition = glm::mix(currentFrame.position, nextFrame.position, blendFactor);
                glm::quat blendedRotation = glm::slerp(currentFrame.rotation, nextFrame.rotation, blendFactor);
                glm::vec3 blendedScale = glm::mix(currentFrame.scale, nextFrame.scale, blendFactor);

                translation = glm::translate(glm::mat4(1.0f), blendedPosition);
                rotation = glm::toMat4(blendedRotation);
                scale = glm::scale(glm::mat4(1.0f), blendedScale);

                nodeTransform = translation * rotation * scale;
            }
        }
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    // Update final bone matrix if this is a bone
    auto it = m_boneMapping.find(nodeName);
    if (it != m_boneMapping.end()) {
        m_finalBoneMatrices[it->second] = globalTransform * currentBoneIt->second.offset;
    }

    // Recursively update child nodes
    aiNode* node = m_currentClip->rootNode->FindNode(nodeName.c_str());
    if (node) {
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            calculateBoneTransform(node->mChildren[i]->mName.C_Str(), globalTransform);
        }
    }
}

// Interpolate keyframes with edge handling:
KeyFrame AnimationComponent::interpolateKeyFrames(float animationTime, const std::vector<KeyFrame>& keyFrames) {
    if (keyFrames.empty()) return KeyFrame();
    if (keyFrames.size() == 1) return keyFrames[0];

    // Handle when animationTime is exactly at the first or last keyframe
    if (animationTime <= keyFrames[0].timeStamp) return keyFrames[0];
    if (animationTime >= keyFrames.back().timeStamp) return keyFrames.back();

    // Find surrounding keyframes
    size_t nextFrameIndex = 0;
    for (; nextFrameIndex < keyFrames.size(); nextFrameIndex++) {
        if (keyFrames[nextFrameIndex].timeStamp > animationTime) {
            break;
        }
    }

    size_t prevFrameIndex = nextFrameIndex - 1;

    // Calculate interpolation factor
    float deltaTime = keyFrames[nextFrameIndex].timeStamp - keyFrames[prevFrameIndex].timeStamp;
    float factor = (animationTime - keyFrames[prevFrameIndex].timeStamp) / deltaTime;

    // Interpolate between keyframes
    const KeyFrame& prevFrame = keyFrames[prevFrameIndex];
    const KeyFrame& nextFrame = keyFrames[nextFrameIndex];

    KeyFrame interpolatedFrame;
    interpolatedFrame.position = glm::mix(prevFrame.position, nextFrame.position, factor);
    interpolatedFrame.rotation = glm::slerp(prevFrame.rotation, nextFrame.rotation, factor);
    interpolatedFrame.scale = glm::mix(prevFrame.scale, nextFrame.scale, factor);

    return interpolatedFrame;
}

void AnimationComponent::play(const std::string& clipName, bool loop) {
    auto it = m_animations.find(clipName);
    if (it != m_animations.end()) {
        m_currentClip = it->second.get();
        m_currentTime = 0.0f;
        m_isPlaying = true;
        m_isLooping = loop;
        m_nextClip = nullptr;
        m_currentFadeTime = 0.0f;
    }
}

void AnimationComponent::crossFadeTo(const std::string& clipName, float fadeTime) {
    auto it = m_animations.find(clipName);
    if (it != m_animations.end() && it->second.get() != m_currentClip) {
        m_nextClip = it->second.get();
        m_fadeTime = fadeTime;
        m_currentFadeTime = 0.0f;
    }
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

const std::string& AnimationComponent::getCurrentClipName() const {
    static const std::string empty = "";
    return m_currentClip ? m_currentClip->name : empty;
}

int AnimationComponent::getBoneId(const std::string& boneName) const {
    auto it = m_boneMapping.find(boneName);
    return it != m_boneMapping.end() ? it->second : -1;
}

const glm::mat4& AnimationComponent::getBoneTransform(int boneId) const {
    if (boneId >= 0 && boneId < m_finalBoneMatrices.size()) {
        return m_finalBoneMatrices[boneId];
    }
    static const glm::mat4 identity(1.0f);
    return identity;
}

const glm::mat4& AnimationComponent::getBoneTransform(const std::string& boneName) const {
    return getBoneTransform(getBoneId(boneName));
}