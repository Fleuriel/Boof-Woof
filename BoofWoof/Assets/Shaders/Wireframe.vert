#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;


layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTexCoord;  // Texture coordinate input


void main()
{
    gl_Position = projection * view * vertexTransform * vec4(modelPosition, 1.0f);
  
}