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

layout(location = 0) in vec3 vertColor;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 FragPos;
layout(location = 3) in vec2 TexCoords;



uniform sampler2D texture_diffuse1;
uniform bool lineRender;

out vec4 fragColor;


void main()
{   
    //if(lineRender)
    //{
        //fragColor = vertColor;
        
    //}else{
        //vec3 lightVector = vec3(-2567, 44448, 91008)-FragPos;
        //float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        fragColor = texture(texture_diffuse1, TexCoords);
    //}
}