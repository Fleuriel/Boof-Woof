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

uniform float setFinalAlpha;

// Add a sampler for the texture
uniform sampler2D albedoTexture;
uniform bool useTexture; // Flag to enable/disable texture

vec4 fromLinear(vec4 linearRGB)
{
    bvec3 cutoff = lessThan(linearRGB.rgb, vec3(0.0031308));
    vec3 higher = vec3(1.055)*pow(linearRGB.rgb, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = linearRGB.rgb * vec3(12.92);

    return vec4(mix(higher, lower, cutoff), linearRGB.a);
}

// Converts a color from sRGB gamma to linear light gamma
vec4 toLinear(vec4 sRGB)
{
    bvec3 cutoff = lessThan(sRGB.rgb, vec3(0.04045));
    vec3 higher = pow((sRGB.rgb + vec3(0.055))/vec3(1.055), vec3(2.4));
    vec3 lower = sRGB.rgb/vec3(12.92);

    return vec4(mix(higher, lower, cutoff), sRGB.a);
}

vec3 sRGBToLinear(vec3 sRGB)
{
    return mix(sRGB / 12.92, pow((sRGB + 0.055) / 1.055, vec3(2.4)), lessThan(sRGB, vec3(0.04045)));
}

// Improved specular distribution using GGX
float D_GGX(float NoH, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float denom = (NoH2 * (alpha2 - 1.0) + 1.0);
    return alpha2 / (PI * denom * denom);
}

// Geometry term (Smith)
float G_Smith(float NoV, float NoL, float roughness) {
    float k = pow(roughness + 1.0, 2.0) / 8.0;
    float G1V = NoV / (NoV * (1.0 - k) + k);
    float G1L = NoL / (NoL * (1.0 - k) + k);
    return G1L * G1V;
}

void main() {
    // Normalize vectors
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(viewPos - fragWorldPos); // View direction
    vec3 L = normalize(inputLight - fragWorldPos); // Light direction
    vec3 H = normalize(L + V); // Halfway vector

    // Dot products
    float NoV = max(dot(N, V), 0.001);
    float NoL = max(dot(N, L), 0.001);
    float NoH = max(dot(N, H), 0.001);
    float VoH = max(dot(V, H), 0.001);

    // Calculate the roughness factor (clamp it to avoid any edge cases)
    float roughness = clamp(fragRoughness, 0.001, 1.0);
    
    // Base reflectivity (F0)
    vec3 F0 = mix(vec3(0.04), fragColor.rgb, fragMetallic);

    // Fresnel term (Schlick approximation)
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - VoH, 5.0);

    // Distribution and Geometry terms
    float D = D_GGX(NoH, roughness);
    float G = G_Smith(NoV, NoL, roughness);

    // Specular BRDF (Cook-Torrance)
    vec3 specular = (D * G * F) / max(4.0 * NoV * NoL, 0.001);

    // Diffuse BRDF (Lambertian for non-metals)
    vec3 baseColor = fragColor.rgb;

    // Blend the texture with the color
    if (useTexture) {
        vec4 texColor = texture(albedoTexture, fragTexCoord);
        texColor = toLinear(texColor);


        texColor.rgb = pow(texColor.rgb, vec3(2.2)); // Convert from sRGB to linear

        texColor.a = 2.2;
        
        baseColor *= texColor.rgb;
        
        
        //vec4 texColor = texture(albedoTexture, fragTexCoord);
        //float blendFactor = 0.5; // Example blend factor, can be uniform or derived
        //baseColor = mix(fragColor.rgb, texColor.rgb, blendFactor);
    }

    baseColor *= vec3(fragColor.a);

    vec3 diffuse = (1.0 - F) * (1.0 - fragMetallic) * baseColor / PI;

    // Combine with light
    vec3 lightColor = vec3(5.0); // Intensity of the light source
    vec3 directLight = (diffuse + specular) * NoL * lightColor;

    // Ambient light (for non-metals: based on the base color)
    vec3 ambient = baseColor * 0.2 * (1.0 - fragMetallic);
    
    // Ambient light for metallic materials (based on F0)
    vec3 ambient_metal = F0 * 0.2; // Reflection for metals

    // Combine direct light and ambient light
    vec3 finalColor = directLight + mix(ambient, ambient_metal, fragMetallic);

    // Tonemapping for HDR (Linear to display space)
    //finalColor = finalColor / (finalColor + vec3(1.0)); // simple tonemapping

    // Gamma correction (2.2 gamma)
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    FragColor = vec4(finalColor, setFinalAlpha);
}

