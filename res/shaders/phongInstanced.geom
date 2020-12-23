#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 UV[];
in vec3 worldNormal[];
in vec3 worldPos[];
in int insideCameraFrustum[];

out vec2 uv;
out vec3 world_pos;
out vec3 world_normal;

void main() {
	if ((insideCameraFrustum[0] + insideCameraFrustum[1] + insideCameraFrustum[2]) > 0) {
		for (int i = 0; i < 3; i++) {
			gl_Position = gl_in[i].gl_Position;

			uv = UV[i];
			world_pos = worldPos[i];
			world_normal = worldNormal[i];

			EmitVertex();
		}

		EndPrimitive();
	}
}
