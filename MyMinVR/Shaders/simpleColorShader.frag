#version 410

const vec4 lightPos = vec4(0.0, 2.0, 2.0, 1.0);
const vec4 color = vec4(0.5, 0.5, 0.5, 1.0);

in vec4 pos;
in vec4 norm;
in vec2 TexCoord;
in vec3 outcolor;

out vec4 fragColor;



void main() {
    //float ambient = 0.1;
    //float diffuse = clamp(dot(norm, normalize(lightPos - pos)), 0.0, 1.0);

    //fragColor = (ambient + diffuse) * color;
	fragColor = vec4(outcolor,1);

	//fragColor = vec4(texture( myTextureSampler, TexCoord ).rgb,1);
}