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

#define MAX_LIGHTS 3  // max number of lights

//uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int numLights;

out vec4 fragColor;

in VS_OUT {
	vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[MAX_LIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

void main()
{

    //if(textureCount ==2 ){
        //vec3 normal = texture(texture_normal1, TexCoords).rgb;
        //
        //normal = normalize(normal * 2.0 - 1.0);
        //
        //vec3 color = texture(texture_diffuse1, TexCoords).rgb;
        //vec3 ambient = 0.1 * color;
        //vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
        //
        //float diff = max(dot(normal, lightDir), 0.0);
        //vec3 diffuse = diff * color;
        //
        //vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
        //vec3 reflectDir = reflect(-lightDir, normal);
        //vec3 halfwayDir = normalize(lightDir + viewDir);
        //float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        //vec3 specular = vec3(0.2) * spec;
        //fragColor = vec4(ambient + diffuse + specular, 1.0);

    //}else 
    if(textureCount == 1 )
	{
		//vec3 lightVector = lightPos - FragPos;
        //float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        vec4 textureColor = texture(texture_diffuse1, TexCoords);
        textureColor.rgb = pow(textureColor.rgb, vec3(1.0/2.2));
        //fragColor = vec4(textureColor.rgb, textureColor.a);
        
        vec3 ambientColor = vec3(0.0f,0.0f,0.0f);
        vec3 diffuseColor = textureColor.rgb;


        vec3 ambient = ambientColor  * 0.1f;
        vec3 result = ambient;

        for (int i = 0; i < numLights; i++) {
            // Calculate the diffuse component
            float diff = max(dot(normal, normalize(fs_in.TangentLightPos[i])), 0.0);
            vec3 diffuse = diff * lights[i].color * lights[i].intensity;

            result += diffuse;  // Sum the diffuse components
        }

        // Output the final color
        
        vec3 fragColor = vec4(result, 1.0); // Combine ambient and diffuse components
        if(lightOn){
            fragColor = vec4(finalColor, 1.0);
        }else{
			fragColor = vec4(textureColor);
		}
        
	}else{
        vec3 lightVector = lightPos - FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        fragColor = vec4(vertColor*N_dot_L, 1.0f);
	}

       
   
}

