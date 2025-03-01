/*!*****************************************************************************
\file               model.vert
\author             Angus TAN Yit Hoe 		
\co-author			Aaron CHAN Jun Xiang	
\date               23/09/2024
\brief              This file contains the Fragment Shader for model                                    
*******************************************************************************/
#version 450 core


layout (location=0) in vec3 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=2) in vec2 aVertexTexture;

layout (location=0) out vec3 vColor;
layout (location=1) out vec2 vTex;



uniform mat4 vertexTransform;
uniform vec3 objectColor;


void main(void){

	gl_Position =  vertexTransform * vec4( aVertexPosition, 1.0f );
    //set the color
	vColor = aVertexColor;
	
	// Set the texture to the 'bytes'
	vTex = aVertexTexture;

}