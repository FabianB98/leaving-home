#version 330 core

in vec3 tex_coords;

// skybox cubemap texture input
uniform samplerCube skybox;

uniform float time;
uniform vec3 sunColor;

// Fragment shader output
out vec4 color;

float nightday;
float tw = .4;
vec4 baseColor1 = vec4(.184, .553, .996, 1);
vec4 baseColor2 = vec4(.184, .531, .99, 1);
vec4 dawnColor = vec4(1, .5, 0, 1);
vec4 starColor = vec4(1);

float smoothcurve(float center, float radius, float x) {
	float start = center - radius;
	float end = center + radius;
	return smoothstep(start, center, x) * smoothstep(end, center, x);
}

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

vec4 mixWithStripes(float vStripe, vec4 c1, vec4 c2) {
	return vStripe * c1 + (1 - vStripe) * c2;
}

vec4 stripeColor(vec4 base, float brightness, float theta) {
	return base + (vec4(1) - base) * theta * brightness * smoothstep(0, tw, nightday);
}

vec4 twilightColor(float theta) {
	return smoothcurve(0, tw, nightday) * 2 * theta * dawnColor;
}

void main() {
	nightday = cos(time);

	float theta = asin(tex_coords.y / length(tex_coords)) / (3.14 * 0.5);
	theta = max(0, min(1, .3 - theta));


	vec4 tex = texture(skybox, tex_coords);
	vec4 texRotated = texture(skybox, vec3(rotationX(time) * vec4(tex_coords, 1)));
	float vStripe = tex.r;
	float vStars = texRotated.g;
	float vSun = texRotated.b;

	vec4 color1 = baseColor1 * (.18 + (1-.18) * smoothstep(-tw, tw, nightday));
	vec4 color2 = baseColor2 * (.15 + (1-.15) * smoothstep(-tw, tw, nightday));

	vec4 stars = smoothstep(0, -tw, nightday) * vStars * starColor;
	vec4 stripe = mixWithStripes(vStripe, 
		stripeColor(color1, 1.5, theta), stripeColor(color2, 1.3, theta));
	vec4 twilight = mixWithStripes(vStripe,
		0.95 * twilightColor(theta), twilightColor(theta));

	vec4 sun = 0.5 * vSun * vec4(sunColor, 1);

	color = stripe + stars + twilight + sun;
	color.a = 1;
}
