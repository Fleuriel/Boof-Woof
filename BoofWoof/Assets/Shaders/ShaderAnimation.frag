#version 450 core

//in vec3 vNormal;      
in vec3 vPosition;    



layout(location = 0) in vec3 vertColor;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec3 FragPos;


struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform bool lightOn;

 //uniform vec3 uLightPosition;
 //uniform vec3 uViewPosition;   
 //uniform vec3 uLightColor;     
 //uniform vec3 uAmbientColor;   
 //

#define NUM_LIGHTS 8  // Define the number of lights you want
uniform Light lights[NUM_LIGHTS];
uniform int numLights;
 in VS_OUT {
	vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos[NUM_LIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;



out vec4 FragColor;


uniform vec3 viewPos;



void main() {
  //  vec3 normal = normalize(vNormal);
  //  vec3 lightDir = normalize(uLightPosition - vPosition);

  //  float diff = max(dot(normal, lightDir), 0.0);
  //  vec3 diffuse = diff * uLightColor;

 //   vec3 ambient = uAmbientColor;

  //  vec3 result = ambient + diffuse;
    FragColor = vec4(1.0f,1.0f,1.0f, 1.0);
}