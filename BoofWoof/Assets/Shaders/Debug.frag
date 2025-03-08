/*!*****************************************************************************
\file               Debug.frag
\author             Aaron CHAN Jun Xiang 		
\date               04/03/2025 [4 March 2025]
\brief              This file contains the Fragment Shader for drawing AABB
*******************************************************************************/

#version 450 core

in vec3 vColor; // Receiving color from vertex shader
out vec4 fFragColor; // Output final color

uniform float gammaValue = 1.0; // Default gamma value to prevent issues

void main() {
    vec4 setColor = vec4(vColor, 1.0);
    fFragColor = setColor;

    // Apply gamma correction if needed
    fFragColor.rgb = pow(fFragColor.rgb, vec3(1.0 / gammaValue));
}
