#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

// Transform matrices as uniform inputs
uniform mat4 T_MVP;
uniform mat4 T_M;
uniform mat3 T_Normal;

out vec2 UV;
out vec3 world_normal;
out vec3 world_pos;

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	UV = vertexUV;

	// output position and normal in world space for lighting calculations in
	// the fragment shader
	world_pos = (T_M * vec4(vertexPos, 1)).xyz;
	world_normal = normalize(T_Normal * normal);
}
