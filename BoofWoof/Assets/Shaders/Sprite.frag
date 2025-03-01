/*!*****************************************************************************
\file               sprite.frag
\author             Angus TAN Yit Hoe 		
\co-author			Aaron CHAN Jun Xiang	
\co-author			Mark LOW Wang Chun
\date               05/04/2024 [5 April 2024]
\brief              This file contains the Fragment Shader for sprites                                    
*******************************************************************************/
#version 450 core

layout (location=0) in vec3 vInterpColor;
layout (location=1) in vec2 vTexCoord;


in vec3 FragPos;
layout (location=0) out vec4 fFragColor;

uniform sampler2D uTex2d;
uniform float opacity;

void main () {
	vec4 frag_color = texture(uTex2d, vTexCoord);
	frag_color = frag_color * vec4(1.0f,1.0f,1.0f,opacity);
	fFragColor = frag_color;
}