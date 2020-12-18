#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 10) in mat4 T_MVP;
layout(location = 14) in uvec2 cellIdAndType;


out vec3 vertexColor;

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	int cellID = int(cellIdAndType.x);
	int r = (cellID >> 16) & 0xff;
	int g = (cellID >> 8) & 0xff;
	int b = (cellID >> 0) & 0xff;
	vertexColor = vec3(r / 255.0, g / 255.0, b / 255.0);
}
