#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{    
    vec3 sampled = texture(text, TexCoords).rgb;
    float luminance = median(sampled.r, sampled.g, sampled.b);
    color = vec4(textColor * luminance, 1.0);

}