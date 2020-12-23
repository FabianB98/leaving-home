#version 330 core

#define MAX_LIGHT_COUNT 16

struct DirectionalLight {
	vec3 intensity;
	vec3 direction;
};

struct PointLight {
	vec3 intensity;
	vec3 position;
};


in vec2 uv; // unused (for now)
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
uniform DirectionalLight directionalLight;
uniform PointLight[MAX_LIGHT_COUNT] pointLights;

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

// Calculate the effect of a directional light on the fragment
vec3 calcDirectionalLight(DirectionalLight light) {
	return calcLight(light.intensity, normalize(light.direction));
}

// Calculate the effect of a point light on the fragment
vec3 calcPointLight(PointLight light) {
	vec3 toLight = light.position - world_pos;
	// dot product for efficient squared distance
	return calcLight(light.intensity, normalize(toLight)) / dot(toLight, toLight);
}

void main() {
	vec3 sum = vec3(0);

	// accumulate the effect of all lights
	sum += calcDirectionalLight(directionalLight);
	for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
		sum += calcPointLight(pointLights[i]);
	}

	color = vec4(sum, 1);
}
