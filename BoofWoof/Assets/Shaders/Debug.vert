/*!*****************************************************************************
\file               Debug.vert
\author             Aaron CHAN Jun Xiang 		
\date               04/03/2025 [4 March 2025]
\brief              This file contains the Vertex Shader for drawing AABB
*******************************************************************************/

#version 450 core

layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aVertexColor;

out vec3 vColor; // Pass the color to the fragment shader

uniform mat4 projection;
uniform mat4 view;
uniform mat4 vertexTransform;

void main(void) {
    gl_Position = projection * view * vertexTransform * vec4(aVertexPosition, 1.0f);
    vColor = aVertexColor; // Use vertex color correctly
}
