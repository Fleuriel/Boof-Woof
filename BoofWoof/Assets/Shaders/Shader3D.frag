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
uniform sampler2D texture_normal1;
uniform int textureCount;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 fragColor;

in VS_OUT {
	vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

void main()
{

    if(textureCount ==2 ){
        vec3 normal = texture(texture_normal1, TexCoords).rgb;

        normal = normalize(normal * 2.0 - 1.0);

        vec3 color = texture(texture_diffuse1, TexCoords).rgb;
        vec3 ambient = 0.1 * color;
        vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * color;

        vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        vec3 specular = vec3(0.2) * spec;
        fragColor = vec4(ambient + diffuse + specular, 1.0);

    }else if(textureCount == 1 )
	{
		vec3 lightVector = lightPos - FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        vec4 textureColor = texture(texture_diffuse1, TexCoords);
        //fragColor = vec4(textureColor.rgb, textureColor.a);
        fragColor = vec4(textureColor.rgb * N_dot_L, textureColor.a);
	}else{
        vec3 lightVector = lightPos - FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        fragColor = vec4(vertColor*N_dot_L, 1.0f);
	}

       
   
}

