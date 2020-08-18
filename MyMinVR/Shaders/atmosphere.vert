#version 410

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

out vec3 v3Pos;
out vec2 TexCoord;

void main() {
    ///v3Pos =  vec3(m * vec4(position, 1.0));
	TexCoord = uv;
    gl_Position = p * v * m * vec4(position, 1.0);
}