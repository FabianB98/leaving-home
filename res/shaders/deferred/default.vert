#version 330 core

#define render geometry

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 T_M;
layout(location = 7) in mat3 T_Normal;
layout(location = 10) in mat4 T_MVP;

layout(location = 14) in uvec2 cellIdAndType;

uniform int pick;

uniform vec3 kA;
out vec3 ambient;
out float e;

out vec2 uv;
out vec3 world_normal;
out vec3 world_pos;

const vec3 highlightColors[3] = vec3[3](
	vec3(0.0, 0.0, 0.0),	// No highlighting
	vec3(0.0, 0.1, 0.0),	// Planned for construction
	vec3(0.1, 0.0, 0.0)		// Planned for destruction
);

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	uv = vertexUV;

	// output position and normal in world space for lighting calculations in
	// the fragment shader
	world_pos = (T_M * vec4(vertexPos, 1)).xyz;
	world_normal = normalize(T_Normal * normal);

	int cellID = int(cellIdAndType.x);
	int highlightID = int(cellIdAndType.y);

	ambient = kA;
	e = 0;

	if ((pick & 0xffffff) == (cellID & 0xffffff)) {
		ambient += vec3(0.1);
		e = 2;
	}

	if (highlightID != 0) {
		ambient += highlightColors[highlightID];
		e = 2;
	}
}

