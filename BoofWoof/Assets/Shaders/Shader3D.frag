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
//uniform vec3 viewPos;

#define NUM_LIGHTS 8  // Define the number of lights you want
uniform Light lights[NUM_LIGHTS];
uniform int numLights;

uniform float gammaValue;

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
        textureColor = texture(texture_diffuse1, TexCoords);
        baseColor.rgb *= textureColor.rgb;
        baseColor.a = 1.0f;
        fragColor = vec4(baseColor);
    }

    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gammaValue));  

    //fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2)); // Apply gamma correction

    // vec3 lightVector = lights[0].position - FragPos;
    // float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
    // fragColor = vec4(vertColor *N_dot_L, 1.0f);
}