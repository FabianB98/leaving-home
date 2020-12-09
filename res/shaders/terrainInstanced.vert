#version 330 core
layout(location = 0) in vec3 vertexPos;
//layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 T_M;
layout(location = 7) in mat3 T_Normal;
layout(location = 10) in mat4 T_MVP;

layout(location = 14) in int cellID;



//out vec2 UV;
out vec3 world_normal;
out vec3 world_pos;

out vec3 kA;
out vec3 kD;
out vec3 kS;
flat out int n;


vec3 grassBase = vec3(0.16863, 0.54902, 0.15294);
vec3 stoneBase = vec3(0.5, 0.5, 0.5);


// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	//UV = vertexUV;

	// output position and normal in world space for lighting calculations in
	// the fragment shader
	world_pos = (T_M * vec4(vertexPos, 1)).xyz;
	world_normal = normalize(T_Normal * normal);


	// we will use this for picking later DON'T DELETE
//	int r = (cellID >> 16) & 0xff;
//	int g = (cellID >> 8) & 0xff;
//	int b = (cellID >> 0) & 0xff;
//	kD = vec3(r / 255.0, g / 255.0, b / 255.0);


	int a = (cellID >> 12) & 0xfff;
	int b = cellID & 0xfff;


	if (world_pos.y <= 24) {
		kA = .0 * grassBase;
		kD = (.3 + 0.1*rand(vec2(float(a), float(b)))) * grassBase;
		kS = .0 * grassBase;
	} else {
		kA = .0 * stoneBase;
		kD = (.3 + 0.1*rand(vec2(float(a), float(b)))) * stoneBase;
		kS = .3 * stoneBase;
	}
	n = 10;
}
