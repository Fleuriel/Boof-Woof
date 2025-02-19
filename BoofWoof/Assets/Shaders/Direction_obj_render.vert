#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;
uniform mat4 lightSpaceMatrix;
uniform vec3 objectColor;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTexCoord;  // Texture coordinate input
//layout(location = 3) in vec3 aTangent;
//layout(location = 4) in vec3 aBitangent;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    bool haveshadow;
    float range;
};

//uniform vec3 lightPos;
uniform vec3 viewPos;

#define NUM_LIGHTS 8  // Define the number of lights you want
uniform Light lights[NUM_LIGHTS];
uniform int numLights;

out layout(location = 0) vec3 vertColor;
out layout(location = 1) vec3 vertNormal; 
out layout(location = 2) vec3 FragPos;
out layout(location = 3) vec2 TexCoords; 
out layout(location = 4) vec4 FragPosLightSpace;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[NUM_LIGHTS];
    vec3 TangentViewPos;
} vs_out;



void main()
{
    gl_Position = projection * view * vertexTransform * vec4(modelPosition, 1.0f);

    vs_out.FragPos = vec3(vertexTransform * vec4(modelPosition, 1.0f));
    vs_out.TexCoords = aTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(vertexTransform)));

    // Compute the normal in world space
    vec3 N = normalize(normalMatrix * vertexNormal);

    // Store necessary values in world space
    for(int i = 0; i < NUM_LIGHTS; i++)
    {
        vs_out.TangentLightPos[i] = lights[i].position; 
    }

    vs_out.TangentViewPos = viewPos;
    FragPosLightSpace =  lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    vertColor = objectColor;
    vertNormal = N; // Normal is now transformed correctly in world space
    TexCoords = aTexCoord;
    FragPos = vs_out.FragPos;
}