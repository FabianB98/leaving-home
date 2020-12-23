#version 330 core

// A small value to help clipping checks when all vertices are almost outside the clipping planes.
const float clippingEpsilon = 2.0f;

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 T_M;
layout(location = 7) in mat3 T_Normal;
layout(location = 10) in mat4 T_MVP;

uniform vec4[6] cameraClippingPlanes;

out vec2 UV;
out vec3 worldNormal;
out vec3 worldPos;
out int insideCameraFrustum;

float distancePointPlane(vec3 point, vec4 plane) {
	return dot(point, plane.xyz) + plane.w;
}

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
}
