/**************************************************************************
 * @file Shader.vert
 * @author 	TAN Angus Yit Hoe

 * @brief
 *
 * This file contains the fragment shader
 *
 *************************************************************************/


#version 450 core


layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}