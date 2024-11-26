#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;  // Position of the light
uniform vec3 viewPos;   // Camera position

out vec4 FragColor;

void main() {
    // Simple lighting calculation (Phong shading)
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);

    // Simple color based on diffuse lighting
    vec3 diffuse = diff * vec3(1.0, 0.5, 0.31);  // Example color (orange)

    FragColor = vec4(diffuse, 1.0);
}
