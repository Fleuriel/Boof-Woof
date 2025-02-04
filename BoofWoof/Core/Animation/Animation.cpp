#include "pch.h"
#include "Animation.h"

bool Animation::LoadModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error: Assimp failed to load model: " << importer.GetErrorString() << std::endl;
        return false;
    }
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        meshes.push_back(scene->mMeshes[i]);
    }
    return true;
}

void Animation::ProcessMesh(aiMesh* mesh, std::vector<AnimVertex>& vertices, std::vector<unsigned int>& indices) {
    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        AnimVertex AnimVertex;
        AnimVertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        AnimVertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            AnimVertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            AnimVertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(AnimVertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Extract bone weights
    ExtractBoneWeights(mesh, vertices);
}

void Animation::ExtractBoneWeights(aiMesh* mesh, std::vector<AnimVertex>& vertices) {
    for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
        std::string boneName = mesh->mBones[i]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName] = { glm::mat4(1.0f) };
            boneCounter++;
        }
        for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; ++j) {
            unsigned int AnimVertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            for (int k = 0; k < 4; ++k) {
                if (vertices[AnimVertexID].boneIDs[k] == -1) {
                    vertices[AnimVertexID].boneIDs[k] = i;
                    vertices[AnimVertexID].weights[k] = weight;
                    break;
                }
            }
        }
    }
}

bool Animation::LoadAnimation(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
    if (!scene || !scene->HasAnimations()) return false;
    ProcessAnimation(scene);
    return true;
}

void Animation::ProcessAnimation(const aiScene* scene) {
    if (!scene->HasAnimations()) return;
    const aiAnimation* aiAnim = scene->mAnimations[0];  // Assuming only one animation
    duration = static_cast<float>(aiAnim->mDuration);
    ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond ? aiAnim->mTicksPerSecond : 25.0f);

    // Process each bone animation
    for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i) {
        aiNodeAnim* channel = aiAnim->mChannels[i];
        BoneAnimation boneAnim;
        boneAnim.boneName = channel->mNodeName.C_Str();

        // Process keyframes for position, rotation, and scale
        for (unsigned int j = 0; j < channel->mNumPositionKeys; ++j) {
            KeyFrame keyFrame;
            keyFrame.timeStamp = static_cast<float>(channel->mPositionKeys[j].mTime);
            keyFrame.position = glm::vec3(channel->mPositionKeys[j].mValue.x,
                channel->mPositionKeys[j].mValue.y,
                channel->mPositionKeys[j].mValue.z);
            boneAnim.keyFrames.push_back(keyFrame);
        }

        for (unsigned int j = 0; j < channel->mNumRotationKeys; ++j) {
            boneAnim.keyFrames[j].rotation = glm::quat(channel->mRotationKeys[j].mValue.w,
                channel->mRotationKeys[j].mValue.x,
                channel->mRotationKeys[j].mValue.y,
                channel->mRotationKeys[j].mValue.z);
        }

        for (unsigned int j = 0; j < channel->mNumScalingKeys; ++j) {
            boneAnim.keyFrames[j].scale = glm::vec3(channel->mScalingKeys[j].mValue.x,
                channel->mScalingKeys[j].mValue.y,
                channel->mScalingKeys[j].mValue.z);
        }

        boneAnimations[boneAnim.boneName] = boneAnim;
    }
}

glm::vec3 Animation::InterpolatePosition(float animationTime, const BoneAnimation& boneAnim) {
    if (boneAnim.keyFrames.size() == 1)
        return boneAnim.keyFrames[0].position;

    // Find the two keyframes for interpolation
    for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
        const KeyFrame& start = boneAnim.keyFrames[i];
        const KeyFrame& end = boneAnim.keyFrames[i + 1];

        if (animationTime < end.timeStamp) {
            float deltaTime = end.timeStamp - start.timeStamp;
            float factor = (animationTime - start.timeStamp) / deltaTime;
            return glm::mix(start.position, end.position, factor);
        }
    }

    return boneAnim.keyFrames.back().position;
}

glm::quat Animation::InterpolateRotation(float animationTime, const BoneAnimation& boneAnim) {
    if (boneAnim.keyFrames.size() == 1)
        return boneAnim.keyFrames[0].rotation;

    // Same approach as position, but use quaternion slerp
    for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
        const KeyFrame& start = boneAnim.keyFrames[i];
        const KeyFrame& end = boneAnim.keyFrames[i + 1];

        if (animationTime < end.timeStamp) {
            float deltaTime = end.timeStamp - start.timeStamp;
            float factor = (animationTime - start.timeStamp) / deltaTime;
            return glm::slerp(start.rotation, end.rotation, factor);
        }
    }

    return boneAnim.keyFrames.back().rotation;
}

glm::vec3 Animation::InterpolateScale(float animationTime, const BoneAnimation& boneAnim) {
    if (boneAnim.keyFrames.size() == 1)
        return boneAnim.keyFrames[0].scale;

    // Same interpolation logic as position
    for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
        const KeyFrame& start = boneAnim.keyFrames[i];
        const KeyFrame& end = boneAnim.keyFrames[i + 1];

        if (animationTime < end.timeStamp) {
            float deltaTime = end.timeStamp - start.timeStamp;
            float factor = (animationTime - start.timeStamp) / deltaTime;
            return glm::mix(start.scale, end.scale, factor);
        }
    }

    return boneAnim.keyFrames.back().scale;
}

const std::vector<glm::mat4>& Animation::GetBoneTransformsAtTime(float currentTime) {
    boneTransforms.clear();
    // Compute transforms for all bones at the current time
    for (const auto& boneAnimPair : boneAnimations) {
        const std::string& boneName = boneAnimPair.first;
        CalculateBoneTransform(currentTime, boneName, glm::mat4(1.0f)); // Start with identity matrix for root
    }
    return boneTransforms;
}

void Animation::CalculateBoneTransform(float animationTime, const std::string& boneName, glm::mat4 parentTransform) {
    // Find the bone animation data
    auto boneAnimIt = boneAnimations.find(boneName);
    if (boneAnimIt == boneAnimations.end()) {
        return; // Bone not found in animation
    }

    const BoneAnimation& boneAnim = boneAnimIt->second;

    // Interpolate position, rotation, and scale
    glm::vec3 position = InterpolatePosition(animationTime, boneAnim);
    glm::quat rotation = InterpolateRotation(animationTime, boneAnim);
    glm::vec3 scale = InterpolateScale(animationTime, boneAnim);

    // Create the local transformation matrix
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), position) *
        glm::mat4_cast(rotation) *
        glm::scale(glm::mat4(1.0f), scale);

    // Combine with the parent's transformation
    glm::mat4 globalTransform = parentTransform * localTransform;

    // Update the bone's final transformation
    if (boneInfoMap.find(boneName) != boneInfoMap.end()) {
        boneTransforms[boneCounter] = globalTransform * boneInfoMap[boneName].offsetMatrix;
    }

    // Recursively process child bones
    for (const auto& childBone : boneAnimations) {
        if (childBone.second.boneName == boneName) {
            CalculateBoneTransform(animationTime, childBone.first, globalTransform);
        }
    }
}