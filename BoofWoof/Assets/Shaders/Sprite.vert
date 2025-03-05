/*!*****************************************************************************
\file               Sprite.vert
\author             Aaron CHAN Jun Xiang 		
\date               14/02/2025 [14 February 2025]
\brief              This file contains the Vertex Shader for sprites                                    
*******************************************************************************/

#version 450 core

layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec3 aVertexColor;
layout (location=2) in vec2 aVertexTexture;

layout (location=0) out vec3 vInterpColor; // Match the frag shader
layout (location=1) out vec2 vTexCoord;    // Match the frag shader

uniform int col_To_Draw;
uniform int row_To_Draw;
uniform int cols;
uniform int rows;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void) {
    // Apply transformations: Model -> View -> Projection
    vec4 worldPosition = model * vec4(aVertexPosition, 0.0, 1.0);
    gl_Position = projection * view * worldPosition;

    // Pass vertex color (matches fragment shader input)
    vInterpColor = aVertexColor;

    // Correct UV mapping for sprite sheets
    vec2 spriteSize = vec2(1.0 / cols, 1.0 / rows);
    vec2 spriteOffset = vec2(col_To_Draw, row_To_Draw) * spriteSize;
    vTexCoord = aVertexTexture * spriteSize + spriteOffset;
}
