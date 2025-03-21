#version 450 core
layout(location = 0) in vec3 aPos;

uniform mat4 vertexTransform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vertexTransform * vec4(aPos, 1.0);
}
