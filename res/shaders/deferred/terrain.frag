#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAmbient;
layout (location = 3) out vec3 gDiffuse;
layout (location = 4) out vec4 gSpecular;

in vec2 UV; // unused (for now)
in vec3 world_normal;
in vec3 world_pos;

uniform mat4 T_V;
uniform mat3 T_V_Normal;

// Material parameters: ambient, diffuse, specular, phong exponent
in vec3 kA;
in vec3 kD;
in vec3 kS;
flat in int n;

void main() {
	gPosition = (T_V * vec4(world_pos, 1)).xyz;
	gNormal = T_V_Normal * world_normal;
	gAmbient = kA;
	gDiffuse = kD;
	gSpecular = vec4(kS, float(n));
}
