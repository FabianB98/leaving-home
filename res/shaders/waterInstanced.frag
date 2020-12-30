#version 330 core

#define MAX_LIGHT_COUNT 2

struct DirectionalLight {
	vec3 intensity;
	vec3 direction_world;
	vec3 direction_view;
};

in vec3 world_normal;
in vec3 world_pos;

// Camera position in world space
uniform vec3 cameraPos;

// Material parameters: ambient, diffuse, specular, phong exponent
uniform vec3 kA;
uniform vec3 kD;
uniform vec3 kS;
uniform int n;

// Lights in the scene
uniform DirectionalLight[MAX_LIGHT_COUNT] directionalLights;

// Fragment shader output
out vec4 color;

// Calculate the effect of a light on the fragment (direction must be normalized)
vec3 calcLight(vec3 intensity, vec3 direction) {
	float cosTheta = max(0, dot(world_normal, direction));
	vec3 cameraDir = normalize(cameraPos - world_pos);
	vec3 reflected = reflect(-direction, world_normal);
	float cosAlpha = max(0, dot(cameraDir, reflected));

	vec3 ambient = kA;
	vec3 diffuse = kD * cosTheta;
	vec3 specular = kS * pow(cosAlpha, n);

	return intensity * (ambient + diffuse + specular);
}

void main() {
	vec3 sum = vec3(0);

	// accumulate the effect of all lights
	for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
		DirectionalLight light = directionalLights[i];
		sum += calcLight(light.intensity, normalize(light.direction_world));
	}

	color = vec4(sum, 0.625);
}