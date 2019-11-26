#version 410

layout (location  = 0 )in vec3 vp;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;


void main () {
	//gl_PointSize = 10.0;
	gl_Position = p * v * m * vec4(vp,1);
}