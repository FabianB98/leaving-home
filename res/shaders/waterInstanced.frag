#version 330 core

#define MAX_LIGHT_COUNT 2

#define SHADOW_BIAS 0.001f

struct DirectionalLight {
	vec3 intensity;
	vec3 direction_world;
	vec3 direction_view;
};

in vec3 world_normal;
in vec3 world_pos;

// Shadow mapping
uniform sampler2DShadow shadowMap[2];
uniform mat4 T_SV[2];

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

float getVisibility(vec4 viewPos, float cosTheta) {
	vec4 shadowPos;

	// choose between the big and small shadow map
	vec4 shadowPos0 = T_SV[0] * viewPos;
	float maxCoord = max(abs(shadowPos0.x), max(abs(shadowPos0.y), abs(shadowPos0.z)));
	bool useSmallMap = maxCoord <= 1;
	if (useSmallMap) {
		shadowPos = shadowPos0;
	} else {
		shadowPos = T_SV[1] * viewPos;
	}

	shadowPos.xyz = shadowPos.xyz * vec3(.5f) + vec3(.5f);
	float bias = SHADOW_BIAS * (1.f + min(2f, 0.01f * abs(viewPos.z)));// * tan(acos(cosTheta));

	float shadow = 0.f;
	shadow += texture(shadowMap[0], vec3(shadowPos.xy, shadowPos.z - bias));

	return shadow;
}

// Calculate the effect of a light on the fragment (direction must be normalized)
vec3 calcLight(vec3 intensity, float shadow, vec3 direction) {
	float cosTheta = max(0, dot(world_normal, direction));
	vec3 cameraDir = normalize(cameraPos - world_pos);
	vec3 reflected = reflect(-direction, world_normal);
	float cosAlpha = max(0, dot(cameraDir, reflected));
	
	float visibility = shadow * getVisibility(vec4(world_pos, 1), cosTheta) + (1.0 - shadow);

	vec3 ambient = kA;
	vec3 diffuse = kD * cosTheta;
	vec3 specular = kS * pow(cosAlpha, n);

	return intensity * (ambient + diffuse + visibility * (specular));
}

void main() {
	vec3 sum = vec3(0);

	// accumulate the effect of all lights (lights[0] is affected by the shadow map)
	DirectionalLight light = directionalLights[0];
	sum += calcLight(light.intensity, 1.f, normalize(light.direction_world));

	for (int i = 1; i < MAX_LIGHT_COUNT; i++) {
		light = directionalLights[i];
		sum += calcLight(light.intensity, 0.f, normalize(light.direction_world));
	}

	color = vec4(sum, 0.625);
}