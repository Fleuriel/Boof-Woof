#version 330 core

layout (location = 0) in vec3 aPosition;   // Vertex position
layout (location = 1) in vec3 aNormal;     // Vertex normal
layout (location = 2) in mat4 aBoneMatrices[MAX_BONES];  // Bone matrices
layout (location = 3) in vec4 aBoneWeights; // Weights for bones

uniform mat4 model;   // Model matrix
uniform mat4 view;    // View matrix
uniform mat4 projection;  // Projection matrix

out vec3 FragPos;
out vec3 Normal;

void main() {
    // Apply bone transformations to the vertex position
    vec4 skinnedPosition = vec4(0.0);
    vec4 skinnedNormal = vec4(0.0);

    // Iterate over all bones affecting this vertex
    for (int i = 0; i < MAX_BONES; ++i) {
        float weight = aBoneWeights[i];
        if (weight > 0.0) {
            skinnedPosition += aBoneMatrices[i] * vec4(aPosition, 1.0) * weight;
            skinnedNormal += aBoneMatrices[i] * vec4(aNormal, 0.0) * weight;
        }
    }

    // Final position and normal after applying the bone transformations
    FragPos = vec3(model * skinnedPosition);
    Normal = normalize(mat3(model) * vec3(skinnedNormal));

    // Apply the final transformation (Model -> View -> Projection)
    gl_Position = projection * view * FragPos;
}
