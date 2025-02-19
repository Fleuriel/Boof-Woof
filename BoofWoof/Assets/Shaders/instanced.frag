
#version 450 core
out vec4 fragColor;

layout(location = 0) in float visibility;

uniform vec4 particleColor;
uniform float gammaValue;

void main()
{
      if(visibility < 0.5f){
            fragColor = vec4(1.0, 0.0, 0.0, 0.0); 
      }else{ 
            fragColor = particleColor; // Solid color with full opacity
      }  

      fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gammaValue)); 
}
