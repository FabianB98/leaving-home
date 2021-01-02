#version 400 core

in vec2 uv;

uniform sampler2DMS depth;

out float color;

void main() {
	color = texelFetch(depth, ivec2(gl_FragCoord), gl_SampleID).r;
}
