#version 450 core

layout(location = 0) in vec3 vertColor;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 FragPos;
layout(location = 3) in vec2 TexCoords;



uniform sampler2D texture_diffuse1;
uniform bool lineRender;

out vec4 fragColor;


void main()
{   
    //if(lineRender)
    //{
        //fragColor = vertColor;
        
    //}else{
        vec3 lightVector = vec3(-2567, 44448, 91008)-FragPos;
        float N_dot_L = max( dot( normalize(vertNormal), normalize(lightVector)), 0.0f );
        vec4 textureColor = texture(texture_diffuse1, TexCoords);
        fragColor = vec4(textureColor.rgb * N_dot_L, textureColor.a);
    //}
}