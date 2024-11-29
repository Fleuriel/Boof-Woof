#version 330 core

in vec2 fragTexCoord;  // Receive UV coordinates from the vertex shader
out vec4 FragColor;

uniform sampler2D albedoTexture;  // Bind the albedo texture
//uniform bool useTexture;


void main()
{
    vec4 textureColor = texture(albedoTexture, fragTexCoord);  // Sample the texture
    FragColor = textureColor;  // Use the sampled texture color
}