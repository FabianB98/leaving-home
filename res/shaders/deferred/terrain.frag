#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAmbient;
layout (location = 3) out vec3 gDiffuse;
layout (location = 4) out vec4 gSpecular;

in vec2 UV; // unused (for now)
in vec3 world_normal;
in vec3 world_pos;

// Material parameters: ambient, diffuse, specular, phong exponent
in vec3 kA;
in vec3 kD;
in vec3 kS;
flat in int n;

void main() {
	gPosition = world_pos;
	gNormal = world_normal;
	gAmbient = kA;
	gDiffuse = kD;
	gSpecular = vec4(kS, float(n));
}
