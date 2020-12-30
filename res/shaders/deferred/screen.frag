#version 400 core

#define MAX_LIGHT_COUNT 2

struct DirectionalLight {
	vec3 intensity;
	vec3 direction_world;
	vec3 direction_view;
};

in vec2 uv;

uniform sampler2DMS gPosition;
uniform sampler2DMS gNormal;
uniform sampler2DMS gAmbient;
uniform sampler2DMS gDiffuse;
uniform sampler2DMS gSpecular;
uniform sampler2D ssao;

//uniform vec3 cameraPos;
uniform DirectionalLight[MAX_LIGHT_COUNT] directionalLights;

out vec4 color;


vec3 calcLight(vec3 intensity, vec3 direction) {
	vec3 view_pos = texelFetch(gPosition, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 view_normal = texelFetch(gNormal, ivec2(gl_FragCoord), gl_SampleID).xyz;

	vec3 kA = texelFetch(gAmbient, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kD = texelFetch(gDiffuse, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kS = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).xyz;
	float n = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).w;

	float cosTheta = max(0, dot(view_normal, direction));
	vec3 cameraDir = normalize(-view_pos);
	vec3 reflected = reflect(-direction, view_normal);
	float cosAlpha = max(0, dot(cameraDir, reflected));

	float ao = texture(ssao, uv).r;

	vec3 ambient = kA * ao;
	vec3 diffuse = kD * cosTheta;
	vec3 specular = kS * pow(cosAlpha, n);

	return intensity * (ambient + diffuse + specular);
}

void main() {
	vec3 sum = vec3(0);

	// accumulate the effect of all lights
	for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
		DirectionalLight light = directionalLights[i];
		sum += calcLight(light.intensity, normalize(light.direction_view));
	}

	color = vec4(sum, 1);
}
