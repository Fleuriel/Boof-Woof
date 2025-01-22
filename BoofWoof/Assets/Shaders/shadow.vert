

#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTexCoord;  // Texture coordinate input
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;


uniform bool reverse_normals;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main()
{
   

    vs_out.FragPos = vec3(vertexTransform * vec4(modelPosition, 1.0f));

    if(reverse_normals)
	{
		vs_out.Normal = transpose(inverse(mat3(vertexTransform))) * vertexNormal;
	}
	else
	{
		vs_out.Normal = transpose(inverse(mat3(vertexTransform))) * -vertexNormal;
	}
    
    vs_out.TexCoords = aTexCoord;
    
    gl_Position = projection * view * vertexTransform * vec4(modelPosition, 1.0f);

}