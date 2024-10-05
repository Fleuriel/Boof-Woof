/**************************************************************************
 * @file Shader.vert
 * @author 	TAN Angus Yit Hoe

 * @brief
 *
 * This file contains the fragment shader
 *
 *************************************************************************/


#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;
uniform vec3 objectColor;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTexCoord;  // Texture coordinate input

out layout(location = 0) vec3 vertColor;
out layout(location = 2) vec3 FragPos;

out vec2 TexCoord;  // Pass texture coordinates to the fragment shader

void main()
{
    gl_Position = projection * view * vertexTransform * vec4(modelPosition, 1.0f);
    vertColor = objectColor;
    TexCoord = aTexCoord;  // Pass actual texture coordinates
    FragPos = vec3(vertexTransform * vec4(modelPosition, 1.0f));
}