#version 450 core

in vec3 vNormal;      
in vec3 vPosition;    

uniform vec3 uLightPosition;
uniform vec3 uViewPosition;   
uniform vec3 uLightColor;     
uniform vec3 uAmbientColor;   

out vec4 FragColor;

void main() {
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPosition - vPosition);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 ambient = uAmbientColor;

    vec3 result = ambient + diffuse;
    FragColor = vec4(1.0f,1.0f,1.0f, 1.0);
}