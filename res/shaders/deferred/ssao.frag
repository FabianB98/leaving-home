#version 400 core

#define TWO_PI 6.2831853

#define SAMPLES 16
#define INTENSITY 1.5

#define SAMPLE_ROTATION 7
#define MAX_MIP 8
#define LOG_MIP_SWITCH 4

#define RADIUS 5f
#define BIAS 0.002f

#define BLUR_Z_FAR 2000.0

in vec2 uv;

uniform sampler2DMS gNormal;
uniform sampler2D gZ;

uniform float screenScale;

uniform vec4 invProjection;


out vec3 color;


vec3 toCameraSpace(vec2 screenPos, float depth) {
	// Eqn. 3 McGuire et al.
	vec2 xy = depth * (invProjection.xz - invProjection.yw * screenPos);
	return vec3(xy, depth);
}

vec3 sampleScreenPos(vec2 screenPos, float screenRadius, int index, float random) {
	float alpha = (index + .5f) / SAMPLES;
	float phi = TWO_PI * alpha * SAMPLE_ROTATION + random;
	alpha = alpha * screenRadius;

	vec2 u = vec2(cos(phi), sin(phi));
	// store screen pos in xy and radius in z
	return vec3(screenPos + u * alpha, alpha);
}

// sum contribution for Eqn. 1
float sampleOcclusion(vec3 samplePos, vec3 camBasePos, vec3 camNormal) {
	// find mip level of depth texture
	int m = clamp(findMSB(int(samplePos.z)) - LOG_MIP_SWITCH, 0, MAX_MIP);
	float z = texelFetch(gZ, ivec2(samplePos.xy) >> m, m).r;

	vec3 camSamplePos = toCameraSpace(samplePos.xy, z);
	vec3 v = camSamplePos - camBasePos;

	float eps = 0.0001;
	return max(0, dot(v, camNormal) + z * BIAS) / (dot(v, v) + eps);
}

vec2 splitZ(float z) {
	float small = clamp(-z / BLUR_Z_FAR, 0.0, 1.0);
	float fix = floor(small * 256.0);
	return vec2(fix / 256.0, small * 256.0 - fix);
}


void main() {
	ivec2 screenPos = ivec2(gl_FragCoord);
	float depth = texelFetch(gZ, screenPos, 0).r; // higher precision than geometry pass position
	vec3 pos = toCameraSpace(screenPos, depth);
	vec3 normal = texelFetch(gNormal, screenPos, gl_SampleID).xyz; // normal should be accurate enough
	
	// pseudo random hash function from Alchemy AO (Eqn. 8 in the paper)
	float random = (3 * screenPos.x ^ screenPos.y + screenPos.x * screenPos.y) * 10;

	// convert world space radius to screen space (Eqn. 5 in the paper)
	float screenRadius = -RADIUS * screenScale / depth;

	float sum = 0.f;
	for (int i = 0; i < SAMPLES; i++) {
		vec3 samplePos = sampleScreenPos(screenPos, screenRadius, i, random);
		sum += sampleOcclusion(samplePos, pos, normal);
	}

	// finalize occlusion estimator
	float occlusion = max(0, 1.f - (2.f * INTENSITY / SAMPLES) * sum);
	occlusion = pow(occlusion, 2);

	// 2x2 blur filter using GPU derivative functions
	// code from McGuire et al.
	ivec2 ssC = ivec2(gl_FragCoord.xy);
	if (abs(dFdx(depth)) < 0.1) {
        occlusion -= dFdx(occlusion) * ((ssC.x & 1) - 0.5);
    }
    if (abs(dFdy(depth)) < 0.1) {
        occlusion -= dFdy(occlusion) * ((ssC.y & 1) - 0.5);
    }

	vec2 storeZ = splitZ(depth);
	color = vec3(occlusion, storeZ.x, storeZ.y);


	// OLD ALGORITHM
	//	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
//	vec3 bitangent = cross(normal, tangent);
//	mat3 TBN = mat3(tangent, bitangent, normal);
//
//	float occlusion = 0;
//
//	for (int i = 0; i < KERNEL_SIZE; ++i) {
//		vec3 samplePos = TBN * samples[i];
//		samplePos = pos + samplePos * RADIUS;
//
//		vec4 offset = T_P * vec4(samplePos, 1.f);
//		offset.xyz /= offset.w;
//		offset.xyz = offset.xyz * 0.5 + vec3(0.5);
//
//		float sampleDepth = texelFetch(gPosition, ivec2(offset.xy * size), gl_SampleID).z;
//		float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(pos.z - sampleDepth));
//		occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.f : 0.f) * rangeCheck;
//	}
//
//	color = 1.0 - (occlusion / KERNEL_SIZE);
//	//color = color * color;
//	color = pow(color, 1.5);
}
