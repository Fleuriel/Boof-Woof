/*!*****************************************************************************
\file               Shader2D.frag
\author             Angus TAN Yit Hoe 		
\co-author			Aaron CHAN Jun Xiang	
\date               23/09/2024
\brief              This file contains the Fragment Shader for Model                                    
*******************************************************************************/

#version 450 core

layout (location=1) in vec2 vTexCoord;

layout (location=0) out vec4 fFragColor;

uniform sampler2D uTex2d;
uniform float opacity;
uniform float gammaValue;


//uniform vec3 textColor;

//uniform int player;


//uniform vec4 inputColor;
//uniform float finalAlpha;
//uniform bool useColor;

void main () {

	vec4 texColor = texture(uTex2d, vTexCoord) * opacity;

//	if(texColor.a < 0.1)
//		discard;
	
	fFragColor = texColor;
	
    fFragColor.rgb = pow(fFragColor.rgb, vec3(1.0 / 2.2f));
}