#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 UV[];
in vec3 worldNormal[];
in vec3 worldPos[];
in int insideCameraFrustum[];

in vec3 k_a[];
in vec3 k_d[];
in vec3 k_s[];
flat in int phongExponent[];

out vec2 uv;
out vec3 world_pos;
out vec3 world_normal;

out vec3 kA;
out vec3 kD;
out vec3 kS;
flat out int n;

void main() {
	if ((insideCameraFrustum[0] + insideCameraFrustum[1] + insideCameraFrustum[2]) > 0) {
		for (int i = 0; i < 3; i++) {
			gl_Position = gl_in[i].gl_Position;

			uv = UV[i];
			world_pos = worldPos[i];
			world_normal = worldNormal[i];

			kA = k_a[i];
			kD = k_d[i];
			kS = k_s[i];
			n = phongExponent[i];

			EmitVertex();
		}

		EndPrimitive();
	}
}
