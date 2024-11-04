
#include "pch.h"
#include "Animation.h"

Animation::Animation(const std::string& filePath)
    : filePath(filePath), duration(0.0f), currentTime(0.0f) {}

bool Animation::LoadAnimationData() {
    // Initialize Assimp Importer
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_LimitBoneWeights);
    
    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "Error loading FBX file: " << importer.GetErrorString() << std::endl;
        return false;
    }
    
    // Process the animations
    if (scene->mNumAnimations > 0) {
        ProcessAnimation(scene->mAnimations[0]);
    }

    // Process the node hierarchy
    ProcessNode(scene->mRootNode, scene);
    
    std::cout << "Loaded animation with duration " << duration << " seconds.\n";
    return true;
}

void Animation::ProcessNode(const aiNode* node, const aiScene* scene) {
    // Check if the node has an associated bone with an animation channel
    if (boneTransforms.find(node->mName.C_Str()) != boneTransforms.end()) {
        // The node name corresponds to a bone with animation data
        boneTransforms[node->mName.C_Str()].boneName = node->mName.C_Str();
    }

    // Recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void Animation::ProcessAnimation(const aiAnimation* animation) {
    duration = static_cast<float>(animation->mDuration / animation->mTicksPerSecond);

    for (unsigned int i = 0; i < animation->mNumChannels; ++i) {
        aiNodeAnim* channel = animation->mChannels[i];
        BoneTransform boneTransform;
        boneTransform.boneName = channel->mNodeName.C_Str();

        // Process position keyframes
        for (unsigned int j = 0; j < channel->mNumPositionKeys; ++j) {
            Keyframe keyframe;
            keyframe.time = static_cast<float>(channel->mPositionKeys[j].mTime / animation->mTicksPerSecond);
            keyframe.position = glm::vec3(channel->mPositionKeys[j].mValue.x, 
                                          channel->mPositionKeys[j].mValue.y, 
                                          channel->mPositionKeys[j].mValue.z);
            boneTransform.keyframes.push_back(keyframe);
        }

        // Process rotation keyframes
        for (unsigned int j = 0; j < channel->mNumRotationKeys; ++j) {
            if (j < boneTransform.keyframes.size()) {
                aiQuaternion rot = channel->mRotationKeys[j].mValue;
                boneTransform.keyframes[j].rotation = glm::quat(rot.w, rot.x, rot.y, rot.z);
            }
        }

        // Process scaling keyframes
        for (unsigned int j = 0; j < channel->mNumScalingKeys; ++j) {
            if (j < boneTransform.keyframes.size()) {
                aiVector3D scale = channel->mScalingKeys[j].mValue;
                boneTransform.keyframes[j].scale = glm::vec3(scale.x, scale.y, scale.z);
            }
        }

        boneTransforms[boneTransform.boneName] = boneTransform;
    }
}

void Animation::Update(float deltaTime) {
    if (duration <= 0) return;
    
    // Loop animation
    currentTime += deltaTime;
    if (currentTime > duration) {
        currentTime = fmod(currentTime, duration);
    }

    for (auto& [boneName, boneTransform] : boneTransforms) {
        Keyframe kf1, kf2;
        bool foundFrames = false;

        for (size_t i = 0; i < boneTransform.keyframes.size() - 1; ++i) {
            if (currentTime >= boneTransform.keyframes[i].time &&
                currentTime <= boneTransform.keyframes[i + 1].time) {
                kf1 = boneTransform.keyframes[i];
                kf2 = boneTransform.keyframes[i + 1];
                foundFrames = true;
                break;
            }
        }

        if (foundFrames) {
            float t = (currentTime - kf1.time) / (kf2.time - kf1.time);
            boneTransforms[boneName].keyframes[0].position = kf1.position * (1.0f - t) + kf2.position * t;
            boneTransforms[boneName].keyframes[0].rotation = glm::slerp(kf1.rotation, kf2.rotation, t);
        }
    }
}

std::unordered_map<std::string, glm::mat4> Animation::GetBoneTransforms() const {
    std::unordered_map<std::string, glm::mat4> transforms;

    for (const auto& [boneName, boneTransform] : boneTransforms) {
        if (boneTransform.keyframes.empty()) continue;

        const Keyframe& kf = boneTransform.keyframes[0];
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), kf.position);
        glm::mat4 rotation = glm::toMat4(kf.rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), kf.scale);

        transforms[boneName] = translation * rotation * scale;
    }

    return transforms;
}

glm::mat4 Animation::InterpolateKeyframes(const Keyframe& kf1, const Keyframe& kf2, float t) {
    glm::vec3 interpolatedPosition = kf1.position * (1.0f - t) + kf2.position * t;
    glm::quat interpolatedRotation = glm::slerp(kf1.rotation, kf2.rotation, t);
    glm::vec3 interpolatedScale = kf1.scale * (1.0f - t) + kf2.scale * t;

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), interpolatedPosition);
    glm::mat4 rotation = glm::toMat4(interpolatedRotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), interpolatedScale);

    return translation * rotation * scale;
}
