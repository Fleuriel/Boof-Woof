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

#define PI 3.14159265359

out vec4 FragColor;

in vec3 fragNormal;
in vec3 fragWorldPos;
in vec4 fragColor; // Diffuse color of the material
in float fragMetallic; // Metallic factor
in float fragRoughness; // Roughness factor
in vec2 fragTexCoord;                   // Interpolated texture coordinates from vertex shader


uniform vec3 inputLight; // Light source position
uniform vec3 viewPos; // Camera position

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

//uniform vec3 lightPos;
uniform vec3 viewPos;

#define NUM_LIGHTS 8  // Define the number of lights you want
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
    bvec3 cutoff = lessThan(linearRGB.rgb, vec3(0.0031308));
    vec3 higher = vec3(1.055)*pow(linearRGB.rgb, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = linearRGB.rgb * vec3(12.92);

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
		    vec3 lightVector = lights[i].position - FragPos;
            float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
            textureColor.rgb = pow(textureColor.rgb, vec3(1.0/2.2));
            //fragColor = vec4(textureColor.rgb, textureColor.a);
            vec3 ambientColor = vec3(0.0f,0.0f,0.0f);
            vec3 diffuseColor = textureColor.rgb;


            vec3 ambient = ambientColor  * 0.1f;
            vec3 diffuse = diffuseColor  * N_dot_L * lights[i].intensity * lights[i].color;

            vec3 finalColor = ambient + diffuse; // Combine ambient and diffuse components
            result += finalColor;
        }
        
        
        if(lightOn){
            fragColor = vec4(result, 1.0);
        }else{
			fragColor = vec4(textureColor);
		}
        
	}else{
        vec3 lightVector = lights[0].position - FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        fragColor = vec4(vertColor*N_dot_L, 1.0f);
	}

       
   
}

