#version 330 core
layout(location = 0) in vec3 vertexPos;

layout(location = 3) in mat4 T_M;
layout(location = 10) in mat4 T_MVP;

uniform float time;

out vec3 worldPos;
out mat4 MVP;



// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}

float height(vec2 pos) {
	return 0.5 * (sin(5*pos.x*rand(pos) + time) + cos(5*pos.y*rand(pos) + time));
}


void main() {
	vec4 pos = vec4(vertexPos, 1);
	vec4 flatWorldPos = T_M * pos;

	pos.y += height(flatWorldPos.xz);
	gl_Position = pos;
	MVP = T_MVP;
	worldPos = (T_M * pos).xyz;
}