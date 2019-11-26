#version 410

const vec3 lightPos = vec3(0.0, 100.0, -100.0);
const vec3 color = vec3(0.5,0.5,0.5);

in vec3 pos;
in vec3 norm;
in vec2 TexCoord;

out vec4 fragColor;

uniform sampler2D myGrassTextureSampler;
uniform sampler2D myMudTextureSampler;
uniform sampler2D myRockTextureSampler;
uniform sampler2D myBlendTextureSampler;

void main() {

   vec4 blendColor = texture( myBlendTextureSampler, TexCoord);
   float backTextureAmmount =  1 - (blendColor.r + blendColor.g + blendColor.b);
   vec2 titledCoords = TexCoord * 40.0;
   vec4 grassColor = texture(myGrassTextureSampler,titledCoords) * backTextureAmmount;
   vec4 mudColor = texture(myMudTextureSampler,titledCoords) * blendColor.r;
   vec4 rockColor = texture(myRockTextureSampler,titledCoords) * blendColor.b;

   vec4 totalColor= grassColor + mudColor + rockColor;

   float ambient = 0.1;
   vec3 Normal = normalize(norm);
   float diffuse = clamp(dot(Normal, normalize(lightPos - pos)), 0.0, 1.0);

   vec3 result = (ambient + diffuse) * vec3(totalColor);
   fragColor = vec4(result, 1.0);
	// fragColor =  texture(myBlendTextureSampler,titledCoords);
}