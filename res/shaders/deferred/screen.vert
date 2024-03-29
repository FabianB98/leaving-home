#version 330 core

#define render lighting

out vec2 uv;

void main() {
	// https://stackoverflow.com/a/51625078
	float x = float(((uint(gl_VertexID) + 2u) / 3u) % 2u); 
    float y = float(((uint(gl_VertexID) + 1u) / 3u) % 2u); 

    gl_Position = vec4(-1.f + x*2.f, -1.f+y*2.f, 0.0f, 1.f);
    uv = vec2(x, y);
}
