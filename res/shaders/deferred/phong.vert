#version 400 core

#define render lighting

layout(location = 0) in vec3 vertexPos;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 T_M;

layout(location = 7) in vec3 light_intensity;
layout(location = 8) in vec3 light_position;
layout(location = 9) in vec3 light_attenuation;

layout(location = 10) in mat4 T_MVP;

out struct PointLight{
	vec3 intensity;
	vec3 position;
	vec3 attenuation;
} pointLight;

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	pointLight.intensity = light_intensity;
	pointLight.position = light_position;
	pointLight.attenuation = light_attenuation;
}
