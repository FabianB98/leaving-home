#version 400 core

#define NOISE_SIZE 32
#define KERNEL_SIZE 32

#define RADIUS 10f
#define BIAS 0.05f

in vec2 uv;

uniform sampler2DMS gPosition;
uniform sampler2DMS gNormal;
uniform sampler2D noiseTex;

//uniform vec3 cameraPos;
uniform vec3 samples[KERNEL_SIZE];
uniform mat4 T_P;

out float color;


void main() {
	vec2 size = textureSize(gPosition);
	vec2 noiseScale = size / NOISE_SIZE;

	vec3 pos = texelFetch(gPosition, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 normal = texelFetch(gNormal, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 randomVec = texture(noiseTex, uv * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0;

	for (int i = 0; i < KERNEL_SIZE; ++i) {
		vec3 samplePos = TBN * samples[i];
		samplePos = pos + samplePos * RADIUS;

		vec4 offset = T_P * vec4(samplePos, 1.f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + vec3(0.5);

		float sampleDepth = texelFetch(gPosition, ivec2(offset.xy * size), gl_SampleID).z;
		float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(pos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.f : 0.f) * rangeCheck;
	}

	color = 1.0 - (occlusion / KERNEL_SIZE);
}
