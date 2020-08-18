#version 410

const vec4 lightPos = vec4(0.0, 2.0, 2.0, 1.0);


in vec4 pos;
in vec4 norm;
in vec2 TexCoord;

out vec4 fragColor;
uniform sampler2D myTextureSampler;
uniform int intersected;
uniform vec3 color;

void main() {
    float ambient = 0.1;
    float diffuse = clamp(dot(norm, normalize(lightPos - pos)), 0.0, 1.0);

    //fragColor = (ambient + diffuse) * color;

	//fragColor = color;

	float alphaV = 1.0f;
	if(intersected != 0)
	{
	  alphaV = 0.5;
	}

	//fragColor.a = alphaV;

	//fragColor = vec4(texture( myTextureSampler, TexCoord ).rgb,alphaV);
	
	fragColor = vec4(color,alphaV);
}