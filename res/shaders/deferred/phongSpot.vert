#version 400 core

#define render lighting

layout(location = 0) in vec3 vertexPos;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 T_M;

layout(location = 7) in vec3 light_intensity;
layout(location = 8) in vec3 light_position;
layout(location = 9) in vec3 light_direction;

layout(location = 10) in mat4 T_MVP;

out struct SpotLight{
	vec3 intensity;
	vec3 position;
	vec3 direction;
	float cutoff;
	float partOfSphere;
} spotLight;

void main() {
	gl_Position = T_MVP * vec4(vertexPos, 1);

	spotLight.intensity = light_intensity;
	spotLight.position = light_position;

	// converting the packed direction back to a vec3 and fov
	float sx = sin(light_direction.x);
	// length(spotLight.direction) is 1 by construction
	spotLight.direction.x = sx * cos(light_direction.y);
	spotLight.direction.y = sx * sin(light_direction.y);
	spotLight.direction.z = cos(light_direction.x);

	float alpha = light_direction.z;
	spotLight.cutoff = cos(alpha);

	// source: wikipedia, spherical cap
	float sa = sin(alpha);
	spotLight.partOfSphere = (1.0 - cos(alpha) + 0.5 * sa * sa) / 2;
}
