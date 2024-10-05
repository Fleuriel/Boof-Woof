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

layout (location=0) out vec3 vColor;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;
uniform vec3 objectColor;


void main(void){

	gl_Position =  projection* view * vertexTransform * vec4( aVertexPosition, 1.0f );
    //set the color
	vColor = aVertexColor;
}