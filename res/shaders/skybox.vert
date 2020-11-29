#version 330 core
layout(location = 0) in vec3 vertexPos;

// Transform matrices as uniform inputs
uniform mat4 T_MVP;

out vec3 tex_coords;

void main() {
	tex_coords = vertexPos;
	gl_Position = (T_MVP * vec4(vertexPos, 1)).xyww;
}
