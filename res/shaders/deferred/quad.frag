#version 400 core

in vec2 uv;

uniform sampler2D image;

out vec4 color;

void main() {
	//float c = ;
	color = texture(image, uv);
}
