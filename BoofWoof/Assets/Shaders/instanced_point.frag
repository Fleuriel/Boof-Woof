
#version 450 core
out vec4 fragColor;

layout(location = 0) in float visibility;

uniform vec3 particleColor;
uniform float opacity;
uniform float gammaValue;

void main()
{
      if(opacity < 0.1) discard; 
      if(visibility < 0.5f){
            fragColor = vec4(1.0, 0.0, 0.0, 0.0); 
      }else{ 
            fragColor = vec4(particleColor,opacity);
      }  

      fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gammaValue)); 
}
