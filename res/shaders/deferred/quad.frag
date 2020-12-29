#version 400 core

#define MAX_LIGHT_COUNT 2

struct DirectionalLight {
	vec3 intensity;
	vec3 direction;
};

in vec2 uv;

uniform sampler2DMS gPosition;
uniform sampler2DMS gNormal;
uniform sampler2DMS gAmbient;
uniform sampler2DMS gDiffuse;
uniform sampler2DMS gSpecular;

uniform vec3 cameraPos;
uniform DirectionalLight[MAX_LIGHT_COUNT] directionalLights;

out vec4 color;


vec3 calcLight(vec3 intensity, vec3 direction) {
	vec3 world_pos = texelFetch(gNormal, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 world_normal = texelFetch(gNormal, ivec2(gl_FragCoord), gl_SampleID).xyz;

	vec3 kA = texelFetch(gAmbient, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kD = texelFetch(gDiffuse, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kS = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).xyz;
	float n = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).w;

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
		sum += calcLight(light.intensity, normalize(light.direction));
	}

	color = vec4(sum, 1);
}
