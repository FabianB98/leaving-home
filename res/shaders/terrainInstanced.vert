#version 330 core

// A small value to help clipping checks when all vertices are almost outside the clipping planes.
const float clippingEpsilon = 2.0f;

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 T_M;
layout(location = 7) in mat3 T_Normal;
layout(location = 10) in mat4 T_MVP;

layout(location = 14) in uvec2 cellIdAndType;

uniform int pick;

uniform vec4[6] cameraClippingPlanes;

out vec2 UV;
out vec3 worldNormal;
out vec3 worldPos;
out int insideCameraFrustum;

out vec3 k_a;
out vec3 k_d;
out vec3 k_s;
flat out int phongExponent;

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

float distancePointPlane(vec3 point, vec4 plane) {
	return dot(point, plane.xyz) + plane.w;
}

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	UV = vertexUV;

	// output position and normal in world space for lighting calculations in
	// the fragment shader
	worldPos = (T_M * vec4(vertexPos, 1)).xyz;
	worldNormal = normalize(T_Normal * normal);

	insideCameraFrustum = 1;
	for (int i = 0; i < 6; i++) {
		if (distancePointPlane(worldPos, cameraClippingPlanes[i]) < -clippingEpsilon) {
			insideCameraFrustum = 0;
		}
	}

	int cellID = int(cellIdAndType.x);
	int a = (cellID >> 12) & 0xfff;
	int b = cellID & 0xfff;

	float r = rand(vec2(float(a), float(b)));

	k_a = ambientCoeffs[cellIdAndType.y] * baseColors[cellIdAndType.y];
	k_d = (diffuseCoeffs[cellIdAndType.y] + 0.1 * r) * baseColors[cellIdAndType.y];
	k_s = specularCoeffs[cellIdAndType.y] * baseColors[cellIdAndType.y];
	phongExponent = 10;

	if ((pick & 0xffffff) == (cellID & 0xffffff))
		k_a = vec3(1, 0, 0);
}
