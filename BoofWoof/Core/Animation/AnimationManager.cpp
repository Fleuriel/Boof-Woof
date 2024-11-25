#include "pch.h"
#include "AnimationManager.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_set>

void AnimationManager::initialize() {
    m_globalSpeedMultiplier = 1.0f;

    // Configure Assimp importer properties
    m_importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    m_importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.0f);
    m_importer.SetPropertyBool(AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, true);
}

void AnimationManager::shutdown() {
    unloadAllAnimations();
    m_importer.FreeScene();
    m_activeComponents.clear();
}

void AnimationManager::update(float deltaTime) {
    // Apply global speed multiplier
    float adjustedDelta = deltaTime * m_globalSpeedMultiplier;

    // Remove expired components
    m_activeComponents.erase(
        std::remove_if(
            m_activeComponents.begin(),
            m_activeComponents.end(),
            [](const std::weak_ptr<AnimationComponent>& comp) {
                return comp.expired();
            }
        ),
        m_activeComponents.end()
    );

    // Update active animation components
    for (auto& weakComp : m_activeComponents) {
        if (auto comp = weakComp.lock()) {
            comp->update(adjustedDelta);
        }
    }
}

bool AnimationManager::loadAnimation(const std::string& filename, const std::string& animName) {
    // Determine animation name
    std::string finalAnimName = animName.empty()
        ? std::filesystem::path(filename).stem().string()
        : animName;

    // Check if animation is already loaded
    if (m_animationLibrary.find(finalAnimName) != m_animationLibrary.end()) {
        std::cout << "Animation '" << finalAnimName << "' already loaded.\n";
        return true;
    }

    // Process the animation file
    return processAnimationFile(filename, finalAnimName);
}

bool AnimationManager::processAnimationFile(const std::string& filename, const std::string& animName) {
    // Import the animation file using Assimp
    const aiScene* scene = m_importer.ReadFile(
        filename,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_LimitBoneWeights |
        aiProcess_GlobalScale |
        aiProcess_PopulateArmatureData
    );

    // Handle errors during file import
    if (!scene || !scene->HasAnimations()) {
        std::cerr << "Failed to load animation file: " << filename << "\n";
        std::cerr << "Assimp error: " << m_importer.GetErrorString() << "\n";
        return false;
    }

    // Process each animation in the file
    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        aiAnimation* anim = scene->mAnimations[i];
        std::string currentAnimName = animName;

        // Append index for multiple animations in a single file
        if (scene->mNumAnimations > 1) {
            currentAnimName += "_" + std::to_string(i);
        }

        // Allocate a new AnimationClip (raw pointer)
        AnimationClip* clip = new AnimationClip();
        clip->name = currentAnimName;
        clip->duration = anim->mDuration;
        clip->ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 24.0f;
        clip->rootNode = scene->mRootNode;

        // Process animation channels
        for (unsigned int j = 0; j < anim->mNumChannels; ++j) {
            aiNodeAnim* channel = anim->mChannels[j];
            BoneInfo& boneInfo = clip->bones[channel->mNodeName.C_Str()];
            boneInfo.name = channel->mNodeName.C_Str();

            // Reserve space for keyframes
            size_t numKeys = std::max({
                channel->mNumPositionKeys,
                channel->mNumRotationKeys,
                channel->mNumScalingKeys
                });
            boneInfo.keyFrames.reserve(numKeys);

            // Process keyframes
            for (unsigned int k = 0; k < numKeys; ++k) {
                KeyFrame keyFrame;

                // Position keys
                if (k < channel->mNumPositionKeys) {
                    auto& pos = channel->mPositionKeys[k];
                    keyFrame.timeStamp = pos.mTime;
                    keyFrame.position = glm::vec3(pos.mValue.x, pos.mValue.y, pos.mValue.z);
                }

                // Rotation keys
                if (k < channel->mNumRotationKeys) {
                    auto& rot = channel->mRotationKeys[k];
                    keyFrame.rotation = glm::quat(rot.mValue.w, rot.mValue.x, rot.mValue.y, rot.mValue.z);
                }

                // Scale keys
                if (k < channel->mNumScalingKeys) {
                    auto& scale = channel->mScalingKeys[k];
                    keyFrame.scale = glm::vec3(scale.mValue.x, scale.mValue.y, scale.mValue.z);
                }

                boneInfo.keyFrames.push_back(keyFrame);
            }
        }

        // Process bone hierarchy and offsets
        std::function<void(aiNode*, const glm::mat4&)> processNode;
        processNode = [&](aiNode* node, const glm::mat4& parentTransform) {
            std::string nodeName = node->mName.C_Str();

            // Convert aiMatrix4x4 to glm::mat4
            glm::mat4 transform;
            memcpy(&transform, &node->mTransformation, sizeof(float) * 16);

            glm::mat4 globalTransform = parentTransform * transform;

            // If this node is a bone in our animation
            if (clip->bones.find(nodeName) != clip->bones.end()) {
                BoneInfo& boneInfo = clip->bones[nodeName];
                boneInfo.offset = globalTransform;
            }

            // Process children
            for (unsigned int i = 0; i < node->mNumChildren; ++i) {
                processNode(node->mChildren[i], globalTransform);
            }
            };

        // Start processing from the root node
        processNode(scene->mRootNode, glm::mat4(1.0f));

        // Store the processed animation
        m_animationLibrary[currentAnimName] = clip;
    }

    return true;
}

AnimationClip* AnimationManager::getAnimation(const std::string& name) {
    auto it = m_animationLibrary.find(name);
    if (it != m_animationLibrary.end()) {
        return it->second;
    }
    return nullptr;
}

void AnimationManager::unloadAnimation(const std::string& name) {
    auto it = m_animationLibrary.find(name);
    if (it != m_animationLibrary.end()) {
        // Check if animation is still in use
        for (const auto& weakComp : m_activeComponents) {
            if (auto comp = weakComp.lock()) {
                if (comp->getCurrentClipName() == name) {
                    std::cout << "Warning: Attempting to unload animation '"
                        << name << "' that is still in use.\n";
                    return;
                }
            }
        }
        delete it->second; // Manually delete the AnimationClip
        m_animationLibrary.erase(it);
    }
}

void AnimationManager::unloadAllAnimations() {
    // Only unload animations that aren't currently in use
    auto it = m_animationLibrary.begin();
    while (it != m_animationLibrary.end()) {
        bool inUse = false;
        for (const auto& weakComp : m_activeComponents) {
            if (auto comp = weakComp.lock()) {
                if (comp->getCurrentClipName() == it->first) {
                    inUse = true;
                    break;
                }
            }
        }
        if (!inUse) {
            delete it->second; // Manually delete the AnimationClip
            it = m_animationLibrary.erase(it);
        }
        else {
            ++it;
        }
    }
}

void AnimationManager::preloadAnimation(const std::string& filename) {
    // Load the animation in a separate thread
    std::thread loadThread([this, filename]() {
        loadAnimation(filename);
        });
    loadThread.detach();
}

void AnimationManager::purgeUnusedAnimations() {
    // Track which animations are currently in use
    std::unordered_set<std::string> inUseAnimations;
    for (const auto& weakComp : m_activeComponents) {
        if (auto comp = weakComp.lock()) {
            inUseAnimations.insert(comp->getCurrentClipName());
        }
    }

    // Remove animations that aren't in use
    auto it = m_animationLibrary.begin();
    while (it != m_animationLibrary.end()) {
        if (inUseAnimations.find(it->first) == inUseAnimations.end()) {
            delete it->second; // Manually delete the AnimationClip
            it = m_animationLibrary.erase(it);
        }
        else {
            ++it;
        }
    }
}

size_t AnimationManager::getTotalMemoryUsage() const {
    size_t totalMemory = 0;

    // Calculate memory used by animation library
    for (const auto& [name, clip] : m_animationLibrary) {
        // Base size of AnimationClip
        totalMemory += sizeof(AnimationClip);

        // Size of name string
        totalMemory += name.capacity() * sizeof(char);

        // Size of bone data
        for (const auto& [boneName, boneInfo] : clip->bones) {
            totalMemory += boneName.capacity() * sizeof(char);
            totalMemory += sizeof(BoneInfo);
            totalMemory += boneInfo.keyFrames.capacity() * sizeof(KeyFrame);
        }
    }

    return totalMemory;
}

void AnimationManager::registerComponent(const std::shared_ptr<AnimationComponent>& component) {
    m_activeComponents.push_back(component);
}
