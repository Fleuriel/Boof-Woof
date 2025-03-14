#version 450 core
layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTexCoord; 

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoord;
    gl_Position = vec4(modelPosition, 1.0);
}