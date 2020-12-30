#version 400 core

in vec2 uv;

uniform sampler2D image;

out float color;

void main() {
	vec2 texelSize = 1.f / textureSize(image, 0);
	float result = 0.f;

	for (int x = -2; x < 2; ++x) {
		for (int y = -2; y < 2; ++y) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(image, uv + offset).r;
		}	
	}

	color = result / (4.0 * 4.0);
}
