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
//uniform int textureCount;



out vec4 fragColor;

void main()
{

    //FragColor = texture(texture1, TexCoord);  // Use only the first texture
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    //FragColor = vec4(vertColor, 1.0f) * vec4(0.5f,0.5f,0.5f,1.f);
    //FragColor = vec4(TexCoord, 0.0f ,1.0f) * vec4(0.5f,0.5f,0.5f,1.f);

    //fragColor = vec4(vertColor, 1.0f) * vec4(0.5f, 0.5f, 0.5f, 1.f);
    //if(textureCount !=0 )
	//{
	//	 fragColor = texture(texture_diffuse1, TexCoords);
	//}else{
    //    vec3 lightVector = vec3(-2567, 44448, 91008)-FragPos;
    //    float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
    //    fragColor = vec4(vertColor*N_dot_L, 1.0f);
	//}

        vec3 lightVector = vec3(-2567, 44448, 91008)-FragPos;
         float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        vec4 textureColor = texture(texture_diffuse1, TexCoords);
        fragColor = vec4(textureColor.rgb * N_dot_L, textureColor.a);
   
}