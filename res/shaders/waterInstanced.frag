#version 330 core

struct DirectionalLight {
	vec3 intensity;
	vec3 direction;
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
uniform DirectionalLight directionalLight;

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

void main() {
	vec3 result = calcDirectionalLight(directionalLight);
	color = vec4(result, 0.625);
}