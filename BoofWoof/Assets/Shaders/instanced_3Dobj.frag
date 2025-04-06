
#version 450 core
out vec4 fragColor;

layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in float outVisibility;


uniform sampler2D texture_diffuse1;
uniform float gammaValue;
uniform float opacity;

void main()
{ //
      //if(outVisibility < 0.5f || opacity < 0.1){
      //      discard;
      ////}else{ 
            fragColor = texture(texture_diffuse1, vTexCoord); // Solid color with full opacity
            //if(fragColor.a < 0.1) discard;
            
      //}  

      fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gammaValue)); 
      fragColor.a = opacity;

}
