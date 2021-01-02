#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAmbient;
layout (location = 3) out vec3 gDiffuse;
layout (location = 4) out vec4 gSpecular;
layout (location = 5) out float gZ;

in vec2 UV; // unused (for now)
in vec3 world_normal;
in vec3 world_pos;

uniform mat4 T_V;
uniform mat3 T_V_Normal;

// Material parameters: ambient, diffuse, specular, phong exponent
uniform vec3 kA;
uniform vec3 kD;
uniform vec3 kS;
uniform int n;

void main() {
	vec3 pos = (T_V * vec4(world_pos, 1)).xyz;
	gPosition = pos;
	gNormal = T_V_Normal * world_normal;
	gZ = pos.z;
	gAmbient = kA;
	gDiffuse = kD;
	gSpecular = vec4(kS, float(n));
}
