#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 vertexTransform;  // This is the model matrix

void main()
{
    // Transform vertex from model space to light space
    gl_Position = lightSpaceMatrix * vertexTransform * vec4(aPos, 1.0);
}
