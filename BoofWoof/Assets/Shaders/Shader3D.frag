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

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

uniform sampler2D texture1;
uniform int textureCount;



out vec4 fragColor;

void main()
{

    if(textureCount !=0 )
	{
		vec4 texColor = texture(texture1, TexCoords);
        vec3 lightVector = vec3(-2567, 44448, 91008) - FragPos;
        float N_dot_L = max(dot(normalize(vertNormal), normalize(lightVector)), 0.0f);
    
        fragColor = vec4(texColor.rgb * N_dot_L, texColor.a);
	}else{
        vec3 lightVector = vec3(-2567, 44448, 91008)-FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        fragColor = vec4(vertColor*N_dot_L, 1.0f);
	}
   
}