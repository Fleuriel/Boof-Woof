#version 450 core
out vec4 FragColor;

uniform vec3 objectIDColor;

void main()
{
    FragColor = vec4(objectIDColor, 1.0);
}
