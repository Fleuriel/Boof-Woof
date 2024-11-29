#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;  // Add UV mapping input

out vec2 fragTexCoord;  // Pass UV to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragTexCoord = texCoord;  // Pass UV to the fragment shader
    gl_Position = projection * view * model * vec4(position, 1.0f);
}