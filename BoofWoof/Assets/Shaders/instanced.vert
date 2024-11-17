
#version 450 core

uniform mat4 projection;
uniform mat4 view;
//uniform mat4 vertexTransform;

layout(location = 0) in vec3 offset;

void main()
{
    gl_Position = projection * view * vec4(offset, 1.0f);
    gl_PointSize = 10.0; // Set the size of the point, adjust as needed
}

/*uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;
uniform vec3 objectColor;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 offset;
layout(location = 3) in vec2 aTexCoord;


out layout(location = 0) vec3 vertColor;
out layout(location = 2) vec3 FragPos;
out layout(location = 1) vec3 vertNormal; 
out layout(location = 3) vec2 TexCoord; 


void main()
{
    gl_Position = projection * view * vertexTransform * vec4(modelPosition, 1.0f);

    mat3 normalMatrix = transpose(inverse(mat3(vertexTransform)));   

    vertColor = objectColor;
    
    vertNormal = normalMatrix * vertexNormal;
    TexCoord = aTexCoord;
    FragPos = vec3(vertexTransform * vec4(modelPosition + offset, 1.0f));

}*/