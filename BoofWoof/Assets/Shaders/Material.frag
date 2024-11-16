/**************************************************************************
 * @file Shader.frag
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
out vec4 FragColor;

layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 FragPos;

in vec4 outputColor;
in vec3 outputLight;

void main()
{
    vec3 lightVector = outputLight - FragPos;
    float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
    FragColor = vec4(outputColor.rgb *N_dot_L, outputColor.w * 1.0f);
} 