/**************************************************************************
 * @file Shader.vert
 * @author 	TAN Angus Yit Hoe

 * @brief
 *
 * This file contains the fragment shader
 *
 *************************************************************************/


#version 450 core

layout(location = 0) in vec3 aPos;        // Vertex position
layout(location = 1) in vec3 aNormal;     // Vertex normal
layout(location = 2) in vec2 aTexCoord;  // Texture coordinates from model


uniform vec4 inputColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;                      // Model transformation matrix

uniform float metallic;                  // Metallic factor (0.0 - 1.0)
uniform float smoothness;                 // Roughness factor (0.0 - 1.0)

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

out layout(location = 0) vec3 vertColor;
out layout(location = 2) vec3 FragPos;
out layout(location = 1) vec3 vertNormal; 
out layout(location = 3) vec2 TexCoord; 

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[NUM_LIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

void main()
{
    gl_Position = projection * view * vertexTransform * vec4(modelPosition, 1.0f);

    vs_out.FragPos = vec3(vertexTransform * vec4(modelPosition, 1.0f));
    vs_out.TexCoords = aTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(vertexTransform)));

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * vertexNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    for(int i = 0; i < NUM_LIGHTS; i++)
	{
		vs_out.TangentLightPos[i] = TBN * lights[i].position;
	}
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;

   

    vertColor = objectColor;
    
    vertNormal = normalMatrix * vertexNormal;
    TexCoord = aTexCoord;
    FragPos = vec3(vertexTransform * vec4(modelPosition, 1.0f));

    fragColor = inputColor;  // Pass base color to fragment shader
    fragMetallic = metallic;  // Pass metallic to fragment shader
    fragRoughness = smoothness;  // Pass roughness to fragment shader
    fragTexCoord = aTexCoord; // Pass texture coordinates
}