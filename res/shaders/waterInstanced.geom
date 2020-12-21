#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldPos[];

out vec3 world_pos;
out vec3 world_normal;


void main() {
	// calculate face normal and use for all vertices (low-poly style)
	vec3 l1 = worldPos[1] - worldPos[0];
	vec3 l2 = worldPos[2] - worldPos[0];
	world_normal = normalize(cross(l1, l2));

	for (int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position;
		world_pos = worldPos[i];
		EmitVertex();
	}

	EndPrimitive();
}