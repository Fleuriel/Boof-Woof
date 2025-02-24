
#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;

layout(location = 0) in vec3 offset;
layout(location = 1) in float visibility;

layout(location = 0) out float outVisibility;

void main()
{
    outVisibility = visibility;
    gl_Position = projection * view * vertexTransform * vec4(offset, 1.0f);
    
    
}

