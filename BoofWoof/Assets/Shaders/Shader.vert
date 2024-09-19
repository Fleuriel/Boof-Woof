/**************************************************************************
 * @file Shader.vert
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
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

in layout(location = 0) vec3 modelPosition;
in layout(location = 1) vec3 vertexNormal;
//in layout(location = 2) vec3 diffuseColor;

vec3 diffuseColor = vec3( 0.8f, 0.8f, 0.8f );

out layout(location = 0)vec3 vertColor;
out layout(location = 1)vec3 vertNormal;
out layout(location = 2)vec3 FragPos;

void main()
{
    
   
    gl_Position =  projection* view * vertexTransform * vec4( modelPosition, 1.0f );
    vertColor = objectColor;
    vertNormal = vertexNormal;
    FragPos = vec3( vertexTransform * vec4( modelPosition, 1.0f ) );
}
