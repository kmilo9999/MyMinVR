#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

out vec3 pos;
out vec3 norm;
out vec2 TexCoord;

void main() {
    pos = vec3(m * vec4(position, 1.0));
    norm = normal;
	TexCoord = uv;

    gl_Position = p * v * vec4(pos, 1.0);
}