/**************************************************************************
 * @file Shader.vert
 * @author 	TAN Angus Yit Hoe

 * @brief
 *
 * This file contains the fragment shader
 *
 *************************************************************************/


#version 450 core

layout(location = 0) in vec3 aPos;        // Vertex position
layout(location = 1) in vec3 aNormal;     // Vertex normal

uniform vec4 inputColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;                      // Model transformation matrix

uniform float metallic;                  // Metallic factor (0.0 - 1.0)
uniform float smoothness;                 // Roughness factor (0.0 - 1.0)

out vec3 fragNormal;                     // Normal in world space
out vec3 fragWorldPos;                   // Fragment world position
out vec4 fragColor;                      // Base color for the fragment
out float fragMetallic;                  // Metallic value for fragment
out float fragRoughness;                 // Roughness value for fragment

void main() {
    fragNormal = normalize(mat3(transpose(inverse(vertexTransform))) * aNormal);  // Transform normal to world space
    fragWorldPos = vec3(vertexTransform * vec4(aPos, 1.0));  // Transform position to world space
    gl_Position = projection * view * vec4(fragWorldPos, 1.0);  // Final vertex position in clip space

    fragColor = inputColor;  // Pass base color to fragment shader
    fragMetallic = metallic;  // Pass metallic to fragment shader
    fragRoughness = smoothness;  // Pass roughness to fragment shader
}