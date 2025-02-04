#version 450 core

layout(location = 0) in vec3 aPosition;      
layout(location = 1) in vec3 aNormal;        
layout(location = 2) in vec4 aBoneWeights;   
layout(location = 3) in ivec4 aBoneIndices;  

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uBoneTransforms[100];

out vec3 vNormal;       
out vec3 vPosition;     

void main() {
    mat4 boneTransform = aBoneWeights.x * uBoneTransforms[aBoneIndices.x] +
                         aBoneWeights.y * uBoneTransforms[aBoneIndices.y] +
                         aBoneWeights.z * uBoneTransforms[aBoneIndices.z] +
                         aBoneWeights.w * uBoneTransforms[aBoneIndices.w];

    vec4 worldPosition = uModel * boneTransform * vec4(aPosition, 1.0);
    vPosition = worldPosition.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(uModel * boneTransform)));
    vNormal = normalize(normalMatrix * aNormal);

    gl_Position = uProjection * uView * worldPosition;
}