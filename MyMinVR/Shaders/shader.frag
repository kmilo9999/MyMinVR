#version 410

const vec4 lightPos = vec4(0.0, 2.0, 2.0, 1.0);
const vec4 color = vec4(0.5, 0.5, 0.5, 1.0);

in vec4 pos;
in vec4 norm;
in vec2 TexCoord;

out vec4 fragColor;
uniform sampler2D myTextureSampler;
uniform int intersected;

void main() {
    float ambient = 0.1;
    float diffuse = clamp(dot(norm, normalize(lightPos - pos)), 0.0, 1.0);

    fragColor = (ambient + diffuse) * color;
	//fragColor = color;

	float alphaV = 1.0f;
	if(intersected != 0)
	{
	  alphaV = 0.5;
	}
	fragColor = vec4(vec3(1,0,0),alphaV);
}