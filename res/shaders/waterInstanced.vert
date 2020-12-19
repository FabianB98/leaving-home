#version 330 core

// A small value to help clipping checks when all vertices are almost outside the clipping planes.
const float clippingEpsilon = 2.0f;

layout(location = 0) in vec3 vertexPos;

layout(location = 3) in mat4 T_M;
layout(location = 10) in mat4 T_MVP;

uniform float time;
uniform vec4[6] cameraClippingPlanes;

out vec3 worldPos;
out int insideCameraFrustum;

float distancePointPlane(vec3 point, vec4 plane) {
	return dot(point, plane.xyz) + plane.w;
}

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
#define M_PI 3.14159265358979323846
float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}
float rand (vec2 co, float l) {return rand(vec2(rand(co), l));}
float rand (vec2 co, float l, float t) {return rand(vec2(rand(co, l), t));}

float perlin(vec2 p, float dim, float time) {
	vec2 pos = floor(p * dim);
	vec2 posx = pos + vec2(1.0, 0.0);
	vec2 posy = pos + vec2(0.0, 1.0);
	vec2 posxy = pos + vec2(1.0);
	
	float c = rand(pos, dim, time);
	float cx = rand(posx, dim, time);
	float cy = rand(posy, dim, time);
	float cxy = rand(posxy, dim, time);
	
	vec2 d = fract(p * dim);
	d = -0.5 * cos(d * M_PI) + 0.5;
	
	float ccx = mix(c, cx, d.x);
	float cycxy = mix(cy, cxy, d.x);
	float center = mix(ccx, cycxy, d.y);
	
	return center * 2.0 - 1.0;
}

float height(vec2 pos) {
	float p = perlin(pos, 1.0, 0.0);
	return 0.5 * (sin(5*pos.x*p + time) + cos(5*pos.y*p + time));
}


void main() {
	vec4 pos = vec4(vertexPos, 1);
	vec4 flatWorldPos = T_M * pos;

	pos.y += height(flatWorldPos.xz);
	gl_Position = T_MVP * pos;
	worldPos = (T_M * pos).xyz;

	insideCameraFrustum = 1;
	for (int i = 0; i < 6; i++) {
		if (distancePointPlane(worldPos, cameraClippingPlanes[i]) < -clippingEpsilon) {
			insideCameraFrustum = 0;
		}
	}
}