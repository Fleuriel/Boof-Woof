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

layout (location=0) in vec3 vColor;


uniform sampler2D uTex2d;
uniform float opacity;
uniform bool useTexture;



//uniform vec3 textColor;

//uniform int player;



//uniform float finalAlpha;
//uniform bool useColor;

void main () {

	vec4 texColor = texture(uTex2d, vTexCoord) * opacity;


	if(texColor.a < 0.1)
		discard;
//	

	if(useTexture)
	{
		fFragColor = texColor * vec4(vColor, 1.0f);
	}
	else
		fFragColor = vec4(vColor, 1.0f) * opacity;



	
}