#version 400 core

in vec2 uv;

uniform sampler2D image;

out vec4 color;

void main() {
//	float z = texelFetch(image, ivec2(gl_FragCoord) >> 3, 3).r;
//
//	color = vec4(vec3(-z / 500.f), 1);
	
	float value = texture(image, uv).r;
	float zFar = 500.f;
	float zNear = 10.f;
	//value = 2.0 * zNear / (zFar + zNear - value * (zFar - zNear));
	color = vec4(value, value, value, 1);
}
