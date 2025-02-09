
#version 450 core

out vec4 FragColor;
uniform vec3 objectColor;
uniform float gammaValue;

void main()
{
   FragColor=vec4(objectColor,1.0);

   FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gammaValue)); 
}