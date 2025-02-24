
#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoordinate;

layout (location = 3) in vec3 aOffset;
layout (location = 4) in float visibility;

layout (location=1) out vec2 vTexCoord;
layout (location=2) out float outVisibility;

void main()
{
    outVisibility = visibility;
    vTexCoord = aTextureCoordinate;

    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 billboardOffset = aPos.x * camRight 
                         + aPos.y * camUp;   
    vec3 worldpos = billboardOffset + aOffset;

    gl_Position = projection * view  * vertexTransform * vec4(worldpos, 1.0f);

    //gl_Position = projection * view * vertexTransform *vec4(aPos + aOffset, 1.0f);
    
    
}

