
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


// uniform float roughness;   // Scalar roughness value (0 to 1)
// uniform float metallic;    // Scalar metallic value (0 to 1)

uniform float roughness;

// Normal Distribution Function (Trowbridge-Reitz GGX)
float DistributionGGX(float NdotH) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH2 = NdotH * NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

// Geometry Function (Smith's Schlick-GGX)
float GeometrySchlickGGX(float NdotV) {
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
    vec4 textureColor = texture(texture_diffuse1, TexCoords);
    vec3 result = vec3(0.0f,0.0f,0.0f);
    vec4 baseColor = inputColor;
    
    // Base reflectivity (F0)
    vec3 F0 = vec3(0.04f); // Standard for non-metallic surfaces
    F0 = mix(F0, textureColor.rgb, 1.0f);

    vec3 asd = vec3(0.0f,0.0f,0.0f);

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

    result *= baseColor.rgb;
    
    baseColor.rgb = pow(baseColor.rgb, vec3(1.0f/2.2f));

    if(lightOn)
    {
    
        fragColor = vec4(result, 1.0f);
        
    }
    else
    {
        baseColor.rgb *= textureColor.rgb;
        baseColor.a = 1.0f;
        fragColor = vec4(baseColor);
    }



    //fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2)); // Apply gamma correction

    // vec3 lightVector = lights[0].position - FragPos;
    // float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
    // fragColor = vec4(vertColor *N_dot_L, 1.0f);
}