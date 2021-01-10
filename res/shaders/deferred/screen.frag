#version 400 core

#define MAX_LIGHT_COUNT 2

#define TWO_PI 6.2831853
#define MIN_SAMPLES 4
#define ALL_THRESHOLD 0.8
#define SAMPLES 16
#define SAMPLE_ROTATION 7

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
uniform sampler2DShadow shadowMap[2];

uniform float shadowMult[2];

uniform mat4 T_S[2];
uniform DirectionalLight[MAX_LIGHT_COUNT] directionalLights;

float BIAS[2] = float[](0.001, 0.001);

out vec4 color;

vec3 sampleScreenPos(int totalSamples, vec2 screenPos, float screenRadius, int index, float random) {
	float alpha = (index + .5f) / totalSamples;
	float phi = TWO_PI * alpha * SAMPLE_ROTATION + random;
	alpha = alpha * screenRadius;

	vec2 u = vec2(cos(phi), sin(phi));
	// store screen pos in xy and radius in z
	return vec3(screenPos + u * alpha, alpha);
}

float getVisibility(vec4 viewPos, float cosTheta) {
	vec4 shadowPos;

	// choose between the big and small shadow map
	vec4 shadowPos0 = T_S[0] * viewPos;
	float maxCoord = max(abs(shadowPos0.x), max(abs(shadowPos0.y), abs(shadowPos0.z)));
	bool useSmallMap = maxCoord <= 1;
	if (useSmallMap) {
		shadowPos = shadowPos0;
	} else {
		shadowPos = T_S[1] * viewPos;
	}

	//float border = 1.f - smoothstep(0.9, 1.0, max(abs(shadowPos0.x), max(abs(shadowPos0.y), abs(shadowPos0.z))));

	// similar to ssao calculation
	ivec2 screenPos = ivec2(gl_FragCoord);
	float random = (3 * screenPos.x ^ screenPos.y + screenPos.x * screenPos.y) * 10;
	float screenRadius = max(-0.4f / viewPos.z, 0.0025f);
	if (!useSmallMap)
		screenRadius = max(-0.001f / viewPos.z, 0.0001f);

	shadowPos.xyz = shadowPos.xyz * vec3(.5f) + vec3(.5f);
	float bias = BIAS[useSmallMap ? 0 : 1] * (1.f + min(2f, 0.01f * abs(viewPos.z)));// * tan(acos(cosTheta));

	float shadow = 0.f;
	for (int i = 0; i < MIN_SAMPLES; i++){
		vec2 samp = sampleScreenPos(MIN_SAMPLES, shadowPos.xy, screenRadius, i, random).xy;
		shadow += texture(shadowMap[useSmallMap ? 0 : 1], vec3(samp, shadowPos.z - bias));
	}

	float minShadow = shadow / MIN_SAMPLES;
	// check if few samples were enough
	if (max(minShadow, 1.f - minShadow) >= ALL_THRESHOLD) {
		shadow = minShadow;
	} else {
		
		for (int i = 0; i < SAMPLES; i++){
			vec2 samp = sampleScreenPos(SAMPLES, shadowPos.xy, screenRadius, i, random).xy;
			shadow += texture(shadowMap[useSmallMap ? 0 : 1], vec3(samp, shadowPos.z - bias));
		}
		shadow /= (SAMPLES + MIN_SAMPLES);	
	}

	//return border * shadow + (1.f - border);
	return shadowMult[0] * shadow + (1.f - shadowMult[0]); // we can use the first value for both shadow maps
}

vec3 calcLight(vec3 intensity, float shadow, vec3 direction) {
	vec3 view_pos = texelFetch(gPosition, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 view_normal = texelFetch(gNormal, ivec2(gl_FragCoord), gl_SampleID).xyz;

	vec4 ambientTex = texelFetch(gAmbient, ivec2(gl_FragCoord), gl_SampleID);
	vec3 kA = ambientTex.xyz;
	float e = ambientTex.w;
	vec3 kD = texelFetch(gDiffuse, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec4 specTex = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID);
	vec3 kS = specTex.xyz;
	float n = specTex.w;

	float cosTheta = max(0, dot(view_normal, direction));
	vec3 cameraDir = normalize(-view_pos);
	vec3 reflected = reflect(-direction, view_normal);
	float cosAlpha = max(0, dot(cameraDir, reflected));
	
	float visibility = shadow * getVisibility(vec4(view_pos, 1), cosTheta) + (1.0 - shadow);
	float ao = texture(ssao, uv).r;

	vec3 emmisive = kA * e;
	vec3 ambient = kA * ao;
	vec3 diffuse = kD * cosTheta;
	vec3 specular = kS * pow(cosAlpha, n);

	return emmisive + intensity * (ambient + visibility * (diffuse + specular));
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
