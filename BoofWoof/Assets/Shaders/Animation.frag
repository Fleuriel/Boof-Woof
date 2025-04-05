#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;


uniform float gammaValue;

void main()
{    
    vec4 textureColor = texture(texture_diffuse1, TexCoords);



    FragColor = textureColor;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gammaValue));
     
}
