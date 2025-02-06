#version 450 core

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTexCoord;  // Texture coordinate input
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
//layout(location = 2) in vec4 aBoneWeights;   
//layout(location = 3) in ivec4 aBoneIndices;  


struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

out layout(location = 2) vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;
uniform vec3 objectColor;
uniform mat4 uBoneTransforms[100];

out vec3 vNormal;       
out vec3 vPosition;     


#define NUM_LIGHTS 8  // Define the number of lights you want
uniform Light lights[NUM_LIGHTS];
uniform int numLights;

uniform vec3 viewPos;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[NUM_LIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

//out layout(location = 0) vec3 vertColor;
//out layout(location = 2) vec3 FragPos;
//out layout(location = 1) vec3 vertNormal; 


void main() {
   // mat4 boneTransform = aBoneWeights.x * uBoneTransforms[aBoneIndices.x] +
   //                      aBoneWeights.y * uBoneTransforms[aBoneIndices.y] +
   //                      aBoneWeights.z * uBoneTransforms[aBoneIndices.z] +
   //                      aBoneWeights.w * uBoneTransforms[aBoneIndices.w];

    //vec4 worldPosition = uModel * boneTransform * vec4(aPosition, 1.0);
    vec4 worldPosition = vertexTransform * vec4(modelPosition, 1.0);


    gl_Position = projection * view * vertexTransform * vec4(modelPosition, 1.0f);


    vs_out.FragPos = vec3(vertexTransform * vec4(modelPosition, 1.0f));
   // vs_out.TexCoords = aTexCoord;

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



    vPosition = worldPosition.xyz;

//    mat3 normalMatrix = transpose(inverse(mat3(vertexTransform * boneTransform)));
 //   vNormal = normalize(normalMatrix * aNormal);

    
    FragPos = vec3(vertexTransform * vec4(modelPosition, 1.0f));
}