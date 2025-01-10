#version 450 core

in vec3 vNormal;      // Normal passed from the vertex shader
in vec3 vPosition;    // World position passed from the vertex shader

layout(location = 0) uniform vec3 uLightPosition;  // Light source position
layout(location = 1) uniform vec3 uViewPosition;   // Camera position
layout(location = 2) uniform vec3 uLightColor;     // Light color
layout(location = 3) uniform vec3 uAmbientColor;   // Ambient light color

out vec4 FragColor;

void main() {
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPosition - vPosition);

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // Ambient lighting
    vec3 ambient = uAmbientColor;

    // Combine lighting contributions
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
