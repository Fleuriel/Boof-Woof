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
uniform bool lightOn;

struct Light {
    vec3 position;
    //vec3 color;
    //float intensity;
};

//uniform vec3 lightPos;
uniform vec3 viewPos;

#define NUM_LIGHTS 3  // Define the number of lights you want
uniform Light lights[NUM_LIGHTS];
uniform int numLights;


out vec4 fragColor;

in VS_OUT {
	vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[NUM_LIGHTS];
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
        vec3 lightDir = normalize(fs_in.TangentLightPos[0] - fs_in.TangentFragPos);

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
        vec4 textureColor = texture(texture_diffuse1, TexCoords);
        vec3 result = vec3(0.0f,0.0f,0.0f);
        for(int i = 0; i < numLights; i++){
		    vec3 lightVector = fs_in.TangentLightPos[i] - fs_in.TangentFragPos;
            float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
            textureColor.rgb = pow(textureColor.rgb, vec3(1.0/2.2));
            //fragColor = vec4(textureColor.rgb, textureColor.a);
            vec3 ambientColor = vec3(0.0f,0.0f,0.0f);
            vec3 diffuseColor = textureColor.rgb;


            vec3 ambient = ambientColor  * 0.1f;
            vec3 diffuse = diffuseColor  * N_dot_L;

            vec3 finalColor = ambient + diffuse; // Combine ambient and diffuse components
            result += finalColor;
        }
        
        
        if(lightOn){
            fragColor = vec4(result, 1.0);
        }else{
			fragColor = vec4(textureColor);
		}
        
	}else{
        vec3 lightVector = fs_in.TangentLightPos[0] - FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        fragColor = vec4(vertColor*N_dot_L, 1.0f);
	}

       
   
}

