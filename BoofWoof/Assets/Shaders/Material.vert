/**************************************************************************
 * @file Shader.vert
 * @author 	TAN Angus Yit Hoe

 * @brief
 *
 * This file contains the fragment shader
 *
 *************************************************************************/


#version 450 core


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // This can be an identity matrix if no transformations are needed
uniform vec4 inputColor;
uniform vec3 inputLight;

out vec3 vertNormal;
out vec3 FragPos;
out vec4 outputColor;
out vec3 outputLight;

void main() {

    vertNormal = vertexNormal;
    gl_Position = projection * view * vec4(aPos, 1.0);  
    FragPos = vec3(model * vec4(aPos, 1.0));
    outputColor = inputColor;
    outputLight = inputLight;
}


//void main()
//{
//    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
//}