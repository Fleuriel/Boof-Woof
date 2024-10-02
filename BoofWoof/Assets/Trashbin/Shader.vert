/**************************************************************************
 * @file Shader.vert
 * @author 	TAN Angus Yit Hoe
 * @param DP email: tan.a@digipen.edu [0067684]
 * @param Course: CS 350
 * @param Course: Advanced Computer Graphics II
 * @date  06/14/2024 (14 JUNE 2024)
 * @brief
 *
 * This file contains the fragment shader
 *
 *************************************************************************/

#version 450 core

layout (location = 0) in vec4 aVertexPosition;


uniform float size;

uniform mat4 uModel_to_NDC;


void main()
{
	//gl_Position = vec4(uModel_to_NDC * vec4(aVertexPosition * size, 1.0));
  gl_Position = uModel_to_NDC * vec4(aVertexPosition.xyz * size, aVertexPosition.w);
//	gl_Position = vec4(vec3(uModel_to_NDC * vec4(aVertexPosition * size)), 1.0f);

 //   gl_Position = vec4(size * aPos.x, size * aPos.y, size * aPos.z, 1.0f);
}