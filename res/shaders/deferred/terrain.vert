#version 330 core

#define deferred 1

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 T_M;
layout(location = 7) in mat3 T_Normal;
layout(location = 10) in mat4 T_MVP;

layout(location = 14) in uvec2 cellIdAndType;

uniform int pick;

out vec2 uv;
out vec3 world_normal;
out vec3 world_pos;

out vec3 kA;
out vec3 kD;
out vec3 kS;
flat out int n;

// Cell types are either grass (type 0), stone (type 1), snow (type 2) and sand (type 3).
const vec3 baseColors[4] = vec3[4](
	vec3(0.16863, 0.54902, 0.15294),	// Grass
	vec3(0.5, 0.5, 0.5),				// Stone
	vec3(1, 1, 1),						// Snow
	vec3(0.96094, 0.89062, 0.67578)		// Sand
);
const float ambientCoeffs[4] = float[4](
	.1,		// Grass
	.1,		// Stone
	.25,	// Snow
	.1		// Sand
);
const float diffuseCoeffs[4] = float[4](
	.3,		// Grass
	.3,		// Stone
	.3,		// Snow
	.3		// Sand
);
const float specularCoeffs[4] = float[4](
	.0,		// Grass
	.3,		// Stone
	.15,	// Snow
	.0		// Sand
);

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	uv = vertexUV;

	// output position and normal in world space for lighting calculations in
	// the fragment shader
	world_pos = (T_M * vec4(vertexPos, 1)).xyz;
	world_normal = normalize(T_Normal * normal);

	int cellID = int(cellIdAndType.x);
	int a = (cellID >> 12) & 0xfff;
	int b = cellID & 0xfff;

	float r = rand(vec2(float(a), float(b)));

	kA = ambientCoeffs[cellIdAndType.y] * baseColors[cellIdAndType.y];
	kD = (diffuseCoeffs[cellIdAndType.y] + 0.1 * r) * baseColors[cellIdAndType.y];
	kS = specularCoeffs[cellIdAndType.y] * baseColors[cellIdAndType.y];
	n = 10;

	if ((pick & 0xffffff) == (cellID & 0xffffff))
		kA = vec3(1, 0, 0);
}

