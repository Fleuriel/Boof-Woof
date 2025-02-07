#include "pch.h"
#include "Shader.h"
#include <map>
#include <unordered_map>
#include "Mesh.h"
#include "Animation/Animation.h"
#include "ResourceManager/ResourceManager.h"

#include <glm/gtx/string_cast.hpp>

//
//
void Mesh::UpdateVerticesWithBones(
    const std::unordered_map<std::string, BoneAnimation>& boneAnimations,
    const std::vector<Vertex>& bindPoseVertices,
    const std::vector<std::string>& boneNames)
{
    std::vector<Vertex> updatedVertices = bindPoseVertices;

    for (size_t i = 0; i < updatedVertices.size(); ++i) {
        Vertex& vertex = updatedVertices[i];
        glm::vec3 finalPosition(0.0f);

        for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
            if (vertex.m_BoneIDs[j] == -1)
                continue;  // No bone assigned

            int boneIndex = vertex.m_BoneIDs[j];
            if (boneIndex < 0 || boneIndex >= static_cast<int>(boneNames.size())) {
                std::cerr << "Error: boneIndex " << boneIndex
                    << " is out of range (boneNames size: " << boneNames.size() << ")."
                    << std::endl;
                continue;
            }
            std::string boneName = boneNames.at(boneIndex);
            const BoneAnimation& boneAnim = boneAnimations.at(boneName);
            glm::mat4 boneTransform = boneAnim.currentTransform;
            glm::mat4 offset = boneAnim.offsetMatrix;

            // Compute the final transformed position:
            glm::vec3 transformedPos = glm::vec3(boneTransform * offset * glm::vec4(vertex.Position, 1.0f));
            finalPosition += vertex.m_Weights[j] * transformedPos;
        }
        updatedVertices[i].Position = finalPosition;
    }

    // Update GPU buffers with new vertex data
    UpdateMesh(updatedVertices, indices);
}


void Mesh::UpdateVerticesWithBonesCombinedA(float deltaTime,
    const std::unordered_map<std::string, BoneAnimation>& boneAnimations,
    const std::vector<Vertex>& bindPoseVertices,
    const std::vector<std::string>& boneNames)
{
    // --- Step 1: Update Bone Transforms (Skinning Animation Part) ---
    // (This is essentially your Animation::UpdateAnimation code.)
    // You must have a variable for currentTime, ticksPerSecond, and duration.
    // For illustration, we assume these are accessible (they might be members of your Animation class).
    static float currentTime = 0.0f;  // or retrieve from your AnimationComponent
    float ticksPerSecond = 10.0f;     // Set appropriately or retrieve it
    float duration = 150.0f;           // Set appropriately (should not be zero)

    currentTime += deltaTime * ticksPerSecond;
    currentTime = fmod(currentTime, duration); // Loop animation

    std::cout << "Current Time: " << currentTime << std::endl;



    // --- Step 2: Update Vertex Positions Using Bone Transforms ---
    // Start with a copy of the original (bind-pose) vertices.
    std::vector<Vertex> updatedVertices = bindPoseVertices;
   


    
    // Update the bone transforms for each bone in the map.
    // (This loop mimics your Animation::UpdateAnimation function.)
    
    
    for (auto& bonePair : const_cast<std::unordered_map<std::string, BoneAnimation>&>(boneAnimations)) {
        std::string boneName = bonePair.first;
        BoneAnimation& boneAnim = bonePair.second;

        


        if (boneAnim.keyFrames.size() < 2)
            continue;  // Not enough keyframes for interpolation

        // Initialize keyframe variables with defaults
        KeyFrame keyFrameBefore = boneAnim.keyFrames[0];
        KeyFrame keyFrameAfter = boneAnim.keyFrames[1];
        bool foundInterval = false;

        // Find the correct keyframe interval for the current time
        for (size_t i = 0; i < boneAnim.keyFrames.size() - 1; ++i) {
            if (currentTime >= boneAnim.keyFrames[i].timeStamp &&
                currentTime < boneAnim.keyFrames[i + 1].timeStamp)
            {
                keyFrameBefore = boneAnim.keyFrames[i];
                keyFrameAfter = boneAnim.keyFrames[i + 1];
                foundInterval = true;
                break;
            }
        }

        // Fallback: if no valid interval was found, use the last two keyframes.
        if (!foundInterval) {
            keyFrameBefore = boneAnim.keyFrames[boneAnim.keyFrames.size() - 2];
            keyFrameAfter = boneAnim.keyFrames[boneAnim.keyFrames.size() - 1];
        }

        // Avoid division by zero.
        if (fabs(keyFrameAfter.timeStamp - keyFrameBefore.timeStamp) < 1e-5f) {
            std::cerr << "Warning: Keyframes have identical timestamps for bone " << boneName << std::endl;
            continue;
        }

        float alpha = (currentTime - keyFrameBefore.timeStamp) / (keyFrameAfter.timeStamp - keyFrameBefore.timeStamp);
        glm::vec3 interpolatedPosition = glm::mix(keyFrameBefore.position, keyFrameAfter.position, alpha);
        glm::quat interpolatedRotation = glm::slerp(keyFrameBefore.rotation, keyFrameAfter.rotation, alpha);
        glm::vec3 interpolatedScale = glm::mix(keyFrameBefore.scale, keyFrameAfter.scale, alpha);

  //      std::cout << "Bone: " << boneName << " interpolated position: "
  //          << interpolatedPosition.x << ", " << interpolatedPosition.y << ", " << interpolatedPosition.z << std::endl;

        // Update the bone's current transformation.
        boneAnim.currentTransform = glm::translate(glm::mat4(1.0f), interpolatedPosition) *
            glm::mat4_cast(interpolatedRotation) *
            glm::scale(glm::mat4(1.0f), interpolatedScale);



    }


    // For each vertex in the mesh...
    for (size_t i = 0; i < updatedVertices.size(); ++i) {
        Vertex& vertex = updatedVertices[i];
        glm::vec3 finalPosition(0.0f);

        // Loop over bone influences (assume MAX_BONE_INFLUENCE is 4)
        for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
            if (vertex.m_BoneIDs[j] == -1)
                continue; // No bone assigned in this slot

            int boneIndex = vertex.m_BoneIDs[j];
            // Safeguard: check that boneIndex is in range
            if (boneIndex < 0 || boneIndex >= static_cast<int>(boneNames.size())) {
                std::cerr << "Error: boneIndex " << boneIndex << " is out of range (boneNames size: " << boneNames.size() << ")." << std::endl;
                continue;
            }
            std::string boneName = boneNames.at(boneIndex);

         //   std::cout << boneName << '\n';

            // Retrieve the bone animation using the bone name.
            const BoneAnimation& boneAnim = boneAnimations.at(boneName);
            glm::mat4 boneTransform = boneAnim.currentTransform;

            std::cout << "Bone " << boneName << " offset matrix: " << glm::to_string(boneAnim.offsetMatrix) << std::endl;
            std::cout << "Bone " << boneName << " animated transform: " << glm::to_string(boneTransform) << std::endl;

            // Apply the transformation weighted by the bone weight.
            glm::vec3 transformedPos = glm::vec3(boneTransform * boneAnim.offsetMatrix * glm::vec4(vertex.Position, 1.0f));
            finalPosition += vertex.m_Weights[j] * transformedPos;
        }
        updatedVertices[i].Position = finalPosition;

        
    }


    for (int i = 0; i < updatedVertices.size(); ++i)
    {
      //  updatedVertices[i].Position.x = i;
      //  updatedVertices[i].Position.y = i + 1;
      //  updatedVertices[i].Position.z = i + 2;

//		std::cout << updatedVertices[i].Position.x << '\t' << updatedVertices[i].Position.y << '\t' << updatedVertices[i].Position.z << '\n';
    }

    // --- Step 3: Update the GPU Buffers ---
    // This calls your overloaded UpdateMesh function to update VBO/EBO.
    UpdateMesh(updatedVertices, indices);
}





void Mesh::UpdateVerticesWithBonesCombined(float deltaTime,
    const std::unordered_map<std::string, BoneAnimation>& boneAnimations,
    const std::vector<Vertex>& bindPoseVertices,
    const std::vector<std::string>& boneNames)
{
    // --- Step 1: (Assuming boneAnimations are already updated by Animation::UpdateAnimation)
    // We assume deltaTime, currentTime, etc. are handled externally.

    // --- Step 2: Skinning: Update each vertex position based on bone transforms
    std::vector<Vertex> updatedVertices = bindPoseVertices;

    for (size_t i = 0; i < updatedVertices.size(); ++i) {
        Vertex& vertex = updatedVertices[i];
        glm::vec3 finalPosition(0.0f);

        for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
            if (vertex.m_BoneIDs[j] == -1)
                continue;
            int boneIndex = vertex.m_BoneIDs[j];
            if (boneIndex < 0 || boneIndex >= static_cast<int>(boneNames.size())) {
                std::cerr << "Error: boneIndex " << boneIndex << " is out of range." << std::endl;
                continue;
            }
            std::string boneName = boneNames.at(boneIndex);
            const BoneAnimation& boneAnim = boneAnimations.at(boneName);
            glm::mat4 boneTransform = boneAnim.currentTransform;

            std::cout << "Bone " << boneName << " offset matrix: " << glm::to_string(boneAnim.offsetMatrix) << std::endl;
            std::cout << "Bone " << boneName << " animated transform: " << glm::to_string(boneTransform) << std::endl;

            glm::vec3 transformedPos = glm::vec3(boneTransform * glm::vec4(vertex.Position, 1.0f));
            finalPosition += vertex.m_Weights[j] * transformedPos;
        }
        updatedVertices[i].Position = finalPosition;
    }

    // --- Step 3: Update the GPU buffers with the new vertex data.
    UpdateMesh(updatedVertices, indices);
}


void Mesh::UpdateVerticesWithBones_DebugCube()
{
    // Create a copy of the original (bind-pose) vertices.
    // (Alternatively, you can create a new vector for the cube test.)
    std::vector<Vertex> updatedVertices = bindPoseVertices;

    // Ensure we have at least 8 vertices.
    if (updatedVertices.size() < 8) {
        std::cerr << "Not enough vertices to form a cube. Vertex count: "
            << updatedVertices.size() << std::endl;
        return;
    }

    // Define 8 cube corners.
    // These positions will form a cube centered at the origin with side length 2.
    std::vector<glm::vec3> cubeCorners = {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f)
    };

    // For debugging, update the first 8 vertices to the cube corners.
    // If there are more than 8 vertices, you might only want to update these or repeat them.
    for (size_t i = 0; i < std::min(updatedVertices.size(), cubeCorners.size()); ++i) {
        updatedVertices[i].Position = cubeCorners[i];
    }

    // Optional: Print out the new positions for verification.
    for (size_t i = 0; i < 8 && i < updatedVertices.size(); ++i) {
        std::cout << "Cube vertex " << i << ": "
            << updatedVertices[i].Position.x << ", "
            << updatedVertices[i].Position.y << ", "
            << updatedVertices[i].Position.z << std::endl;
    }

    // Update the GPU buffers with the new vertex data.
    UpdateMesh(updatedVertices, indices);
}