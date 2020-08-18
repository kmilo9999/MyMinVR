#version 410


in vec3 v3Pos;


uniform sampler2D myTextureSampler;
in vec2 TexCoord;

out vec4 fragColor;



void main() {
    
	fragColor = vec4(texture( myTextureSampler, TexCoord ).rgb,1);

}