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
//uniform vec3 textColor;

//uniform int player;
//uniform float opacity;




void main () {

	vec4 setColor = {0.0f, 0.0f, 0.0f, 1.0f};
	setColor = texture(uTex2d, vTexCoord);	
	//setColor = setColor * vec4(1.0f,1.0f,1.0f, opacity);


		// For Textures
		fFragColor = setColor;
	
	// For Font Color [Out]
	//fontColor = vec4(textColor,1.0) * setColor;

}