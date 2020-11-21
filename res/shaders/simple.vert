#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

// Transform matrices as uniform inputs
uniform mat4 T_MVP;

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);
}
