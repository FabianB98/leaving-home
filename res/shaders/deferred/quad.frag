#version 400 core

in vec2 uv;

uniform sampler2D image;

out vec4 color;

void main() {
//	float z = texelFetch(image, ivec2(gl_FragCoord) >> 3, 3).r;
//
//	color = vec4(vec3(-z / 500.f), 1);
	
	
	color = texture(image, uv).xxxw;
}
