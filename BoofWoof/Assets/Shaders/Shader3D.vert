#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;
uniform vec3 objectColor;

in layout(location = 0) vec3 modelPosition;
in layout(location = 1) vec3 vertexNormal;
in layout(location = 2) vec2 aTexCoords;


vec3 diffuseColor = vec3( 0.8f, 0.8f, 0.8f );

out layout(location = 0)vec3 vertColor;
out layout(location = 1)vec3 vertNormal;
out layout(location = 2)vec3 FragPos;
out layout(location = 3)vec2 TexCoords;


void main()
{
    
	
   
    gl_Position =  projection* view * vertexTransform * vec4( modelPosition, 1.0f );
    vertColor = objectColor;
    vertNormal = vertexNormal;
    TexCoords = aTexCoords;
    FragPos = vec3( vertexTransform * vec4( modelPosition, 1.0f ) );
}

