#version 400 core

in vec2 uv;

uniform sampler2D image;

uniform vec2 axis;

out vec3 color;

const float gaussian[4 + 1] = 
    float[](0.398943, 0.241971, 0.053991, 0.004432, 0.000134);  // stddev = 1.0


float combineZ(vec2 store) {
    return (store.x * 256.0 + store.y) / 257.0;
}

void main() {
	vec4 value = texelFetch(image, ivec2(gl_FragCoord), gl_SampleID);
	float z = combineZ(value.gb);

	float result = 0.f;
	float sumW = 0.f;

	for (int i = -3; i <= 3; ++i) {
		ivec2 offset = ivec2(axis * 3 * i);

		vec3 offset_val = texelFetch(image, ivec2(gl_FragCoord) + offset, gl_SampleID).rgb;
		float offset_z = combineZ(offset_val.gb);

		float fac = 2000 * abs(z - offset_z);

		float w = gaussian[abs(i)] * max(0, 1.0 - fac);
		sumW += w;
		result += offset_val.r * w;
	}

	color = vec3(result / sumW, value.g, value.b);
}
