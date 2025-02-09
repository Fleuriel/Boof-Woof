#version 450 core

layout (location=0) in vec3 vColor;
layout (location=1) in vec2 vTexCoord;
layout (location=2) in vec3 mColor;

layout (location=0) out vec4 fFragColor;

uniform sampler2D uTex2d;

uniform float gammaValue;


void main () {

	vec4 setColor = {0.0f, 0.0f, 0.0f, 1.0f};

	setColor = vec4(vColor , 1.0f);

	fFragColor = setColor;
	//fontColor = vec4(textColor,1.0) * setColor;

	
	fFragColor.rgb = pow(fFragColor.rgb, vec3(1.0/gammaValue)); 
}