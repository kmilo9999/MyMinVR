#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;


uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

out vec4 pos;
out vec4 norm;
out vec2 TexCoord;
out vec3 outcolor;

void main() {
    pos = v * m * vec4(position, 1.0);
    norm = normalize(v * m * vec4(normal, 0.0));
	TexCoord = uv;
	outcolor = vec3(1,1,1);
    gl_Position = p * pos;
}