
#version 450 core
out vec4 fragColor;

layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in float outVisibility;


uniform sampler2D uTex2d;
uniform float gammaValue;
uniform float opacity;

void main()
{ 
      if(opacity < 0.1) discard; 
      if(outVisibility < 0.5f){
            discard;
      }else{ 
            fragColor = texture(uTex2d, vTexCoord); // Solid color with full opacity
      }  

      fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gammaValue)); 
      fragColor.a = opacity;

}
