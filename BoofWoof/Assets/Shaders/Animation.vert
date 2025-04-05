#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;


struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    bool haveshadow;
    float range;
};

#define NUM_LIGHTS 8  // Define the number of lights you want
uniform Light lights[NUM_LIGHTS];
uniform int numLights;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[NUM_LIGHTS];
    vec3 TangentViewPos;
} vs_out;

uniform vec3 viewPos;


//out layout(location = 0) vec3 vertColor;
out layout(location = 1) vec3 vertNormal; 
out layout(location = 2) vec3 FragPos;
out layout(location = 3) vec2 TexCoords; 
out layout(location = 4) vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];


void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
   }
	
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;

    vs_out.FragPos = vec3( model*vec4(totalPosition,11.0) );
	vs_out.TexCoords = tex;

    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vec3 N = normalize(normalMatrix * vertexNormal);

    for(int i =0; i < NUM_LIGHTS; i++){
        vs_out.TangentLightPos[i] = lights[i].position; 
    }
    vs_out.TangentViewPos = viewPos;

    FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    //vertColor = objectColor;
    vertNormal = N; // Normal is now transformed correctly in world space
    TexCoords = aTexCoord;
    FragPos = vs_out.FragPos;


}
