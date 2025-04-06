#version 450 core
layout(location = 0) in vec3 vertColor;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 FragPos;
layout(location = 3) in vec2 TexCoords;
layout(location = 4) in vec4 FragPosLightSpace;
 

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D shadowMap;
uniform float brightness;

uniform int textureCount;
uniform bool lightOn;


uniform float finalAlpha;

uniform vec4 albedo;

// PBR Uniforms
uniform float u_Metallic;
uniform float u_Roughness;



struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    bool haveshadow;
    float range;
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
} fs_in;

const int POISSON_COUNT = 16; 
const vec2 poissonDisk[POISSON_COUNT] = vec2[](
    vec2(  0.276,  0.117 ), 
    vec2( -0.179,  0.149 ), 
    vec2(  0.196, -0.252 ), 
    vec2( -0.320,  0.350 ), 
    vec2(  0.124,  0.453 ), 
    vec2( -0.038, -0.469 ), 
    vec2(  0.472,  0.118 ), 
    vec2( -0.479, -0.073 ), 
    vec2( -0.105,  0.013 ), 
    vec2( -0.210, -0.379 ), 
    vec2(  0.382, -0.332 ), 
    vec2( -0.435,  0.156 ), 
    vec2(  0.074,  0.299 ), 
    vec2(  0.319,  0.384 ), 
    vec2( -0.303, -0.187 ), 
    vec2(  0.023, -0.097 )
);

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

mat2 getRotationMatrix(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, -s, s, c);
}

float ShadowCalculationPCF(
    sampler2D shadowMap,
    vec4 fragPosLightSpace,
    vec3 fragPos,
    vec3 normal,
    vec3 lightPos)
{
     // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Early out if outside shadow map
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0) 
    {
        return 0.0;
    }

    // Calculate current depth in light’s perspective
    float currentDepth = projCoords.z;

    // Calculate bias
    vec3 lightDir = normalize(lightPos - fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

   float angle = 2.0 * 3.14159 * rand(floor(gl_FragCoord.xy)); 
    mat2 rotation = getRotationMatrix(angle);

  
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int i = 0; i < POISSON_COUNT; i++) {
       
        vec2 offset = rotation * poissonDisk[i] * 1.5; 
        vec2 sampleUV = projCoords.xy + offset * texelSize;
        
        float sampledDepth = texture(shadowMap, sampleUV).r;
        if(currentDepth - bias > sampledDepth) {
            shadow += 1.0; 
        }
    }
    shadow /= float(POISSON_COUNT);

    return shadow;
}


void main()
{
    vec4 textureColor = texture(texture_diffuse1, TexCoords);
    vec3 result = vec3(0.0f,0.0f,0.0f);
    vec4 baseColor = texture(texture_diffuse1, TexCoords);
    
    // Base reflectivity (F0)
    vec3 F0 = vec3(0.04f); // Standard for non-metallic surfaces
    F0 = mix(F0, textureColor.rgb, 1.0f);

    vec3 asd = vec3(0.0f,0.0f,0.0f);

    for(int i = 0; i < numLights; i++){
        vec3 lightVector = lights[i].position - FragPos;
        // if length longer than range continue
        if(length(lightVector)>lights[i].range){
            continue;
        }
        float distance = length(lightVector);
        float t = distance / lights[i].range;
        float attenuation = max(1.0 - t * t, 0.0);
        attenuation *= smoothstep(lights[i].range, lights[i].range * 0.9, distance);
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        textureColor.rgb = pow(textureColor.rgb, vec3(1.0/2.2));
        //fragColor = vec4(textureColor.rgb, textureColor.a);

        // ambient
        vec3 ambientColor = lights[i].color;
        vec3 ambient = ambientColor  * 0.1f* attenuation;

        // diffuse
        vec3 diffuseColor = textureColor.rgb;
        vec3 diffuse = diffuseColor  * N_dot_L * lights[i].intensity * lights[i].color* attenuation;

        vec3 finalColor;
        if(lights[i].haveshadow){
            float shadow = ShadowCalculationPCF(
                                                shadowMap,
                                                FragPosLightSpace,    // or however you store per-light light-space position
                                                FragPos,
                                                normalize(vertNormal),
                                                lights[i].position
                                            );

            finalColor = ambient + diffuse*(1-shadow); // Combine ambient and diffuse components
        }else{
            finalColor = ambient + diffuse;
        }
        result += finalColor* attenuation;
        
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


    fragColor.rgb *= albedo.rgb;

	fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gammaValue)); 
    fragColor.rgb = fragColor.rgb * brightness;

    //fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2)); // Apply gamma correction

    // vec3 lightVector = lights[0].position - FragPos;
    // float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
    // fragColor = vec4(vertColor *N_dot_L, 1.0f);
}