#version 450 core

layout(location = 0) in vec3 aPosition;      // Vertex position
layout(location = 1) in vec3 aNormal;        // Vertex normal
layout(location = 2) in vec4 aBoneWeights;   // Weights for each bone
layout(location = 3) in ivec4 aBoneIndices;  // Indices of bones affecting this vertex

layout(location = 0) uniform mat4 uModel;       // Model matrix
layout(location = 1) uniform mat4 uView;        // View matrix
layout(location = 2) uniform mat4 uProjection;  // Projection matrix
layout(location = 3) uniform mat4 uBoneTransforms[100]; // Bone transformation matrices (max 100)

out vec3 vNormal;       // Pass normal to fragment shader
out vec3 vPosition;     // Pass world position to fragment shader

void main() {
    mat4 boneTransform = aBoneWeights.x * uBoneTransforms[aBoneIndices.x] +
                         aBoneWeights.y * uBoneTransforms[aBoneIndices.y] +
                         aBoneWeights.z * uBoneTransforms[aBoneIndices.z] +
                         aBoneWeights.w * uBoneTransforms[aBoneIndices.w];

    vec4 worldPosition = uModel * boneTransform * vec4(aPosition, 1.0);
    vPosition = worldPosition.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(uModel * boneTransform)));
    vNormal = normalize(normalMatrix * aNormal);

    gl_Position = uProjection * uView * worldPosition;
}