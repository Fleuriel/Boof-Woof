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


uniform float finalAlpha;
uniform vec4 inputColor;



// PBR Uniforms
uniform float u_Metallic;
uniform float u_Roughness;


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


// Constants
const float PI = 3.14159265359;


uniform float roughness;   // Scalar roughness value (0 to 1)
uniform float metallic;    // Scalar metallic value (0 to 1)



// Normal Distribution Function (Trowbridge-Reitz GGX)
float DistributionGGX(float NdotH, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH2 = NdotH * NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

// Geometry Function (Smith's Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

// Fresnel Equation (Schlick's approximation)
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

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

    }else if(textureCount == 1)
    {
        vec4 textureColor = texture(texture_diffuse1, TexCoords);
        vec3 result = vec3(0.0f,0.0f,0.0f);
        vec4 baseColor = inputColor;
        
        // Base reflectivity (F0)
        vec3 F0 = vec3(0.04); // Standard for non-metallic surfaces
        F0 = mix(F0, textureColor.rgb, metallic);

        for(int i = 0; i < numLights; i++){
            vec3 lightVector = lights[i].position - FragPos;
            vec3 N = normalize(vertNormal);
            vec3 L = normalize(lightVector);
            vec3 V = normalize(viewPos - FragPos);
            vec3 H = normalize(L + V);

            // Dot products
            float NdotL = max(dot(N, L), 0.0);
            float NdotH = max(dot(N, H), 0.0);
            float NdotV = max(dot(N, V), 0.0);

            // PBR Calculations
            // Normal Distribution (Specular D)
            float D = DistributionGGX(NdotH, roughness);
            
            // Geometry (Specular G)
            float G = GeometrySchlickGGX(NdotV, roughness) * 
                      GeometrySchlickGGX(NdotL, roughness);
            
            // Fresnel (Specular F)
            vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

            // Specular calculation
            vec3 numerator = D * G * F;
            float denominator = 4.0 * NdotV * NdotL + 0.0001;
            vec3 specular = numerator / denominator;

            // Diffuse calculation
            vec3 kS = F; // Fresnel determines specular reflection
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic; // Reduce diffuse for metals

            // Combine lighting components
            vec3 diffuse = kD * textureColor.rgb / PI;

            vec3 finalColor = (diffuse + specular) 
                * lights[i].color 
                * lights[i].intensity 
                * NdotL;

            result += finalColor;
        }

        result *= baseColor.rgb;
        
        if(lightOn){
            fragColor = vec4(result, finalAlpha);
        }else{
            baseColor.rgb *= textureColor.rgb;
            baseColor.a = finalAlpha;
            fragColor = vec4(baseColor);
        }
    
        
	}else{
        vec3 lightVector = lights[0].position - FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        fragColor = vec4(vertColor*N_dot_L, 1.0f);
	}


    fragColor = vec4(TexCoords,0.0, 1.0f);

       
   
}

