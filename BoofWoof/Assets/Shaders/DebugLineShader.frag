#version 330 core

in vec3 vColor;
out vec4 FragColor;

uniform float gammaValue;

void main()
{
    FragColor = vec4(vColor, 1.0);
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gammaValue)); 
}
