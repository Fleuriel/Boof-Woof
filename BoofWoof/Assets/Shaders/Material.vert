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

 uniform mat4 projection;
 uniform mat4 view;
 uniform mat4 model;
 
 void main() {
     gl_Position = projection * view * vec4(aPos, 1.0);
 }
//void main()
//{
//    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
//}