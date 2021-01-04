#version 400 core

#define MAX_LIGHT_COUNT 2

#define SHADOW_BIAS 0.001f

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
uniform sampler2D shadowMap;

uniform mat4 T_S;
//uniform vec3 cameraPos;
uniform DirectionalLight[MAX_LIGHT_COUNT] directionalLights;

out vec4 color;


float getVisibility(vec4 viewPos) {
	vec4 shadowPos = T_S * viewPos;
	float border = 1.f - smoothstep(0.9, 1.0, max(abs(shadowPos.x), max(abs(shadowPos.y), abs(shadowPos.z))));

	shadowPos.xyz = shadowPos.xyz * vec3(.5f) + vec3(.5f);
	float shadowDepth = texture(shadowMap, shadowPos.xy).x;
	float shadow = shadowDepth < (shadowPos.z - SHADOW_BIAS) ? 0.f : 1.f;

	return border * shadow + (1.f - border);
}

vec3 calcLight(vec3 intensity, float shadow, vec3 direction) {
	vec3 view_pos = texelFetch(gPosition, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 view_normal = texelFetch(gNormal, ivec2(gl_FragCoord), gl_SampleID).xyz;

	float visibility = shadow * getVisibility(vec4(view_pos, 1)) + (1.0 - shadow);

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

	return intensity * (ambient + visibility * (diffuse + specular));
}

void main() {
	vec3 sum = vec3(0);

	// accumulate the effect of all lights (lights[0] is affected by the shadow map)
	DirectionalLight light = directionalLights[0];
	sum += calcLight(light.intensity, 1.f, normalize(light.direction_view));

	for (int i = 1; i < MAX_LIGHT_COUNT; i++) {
		light = directionalLights[i];
		sum += calcLight(light.intensity, 0.f, normalize(light.direction_view));
	}

	color = vec4(sum, 1);
}
