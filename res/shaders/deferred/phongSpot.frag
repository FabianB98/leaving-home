#version 400 core

in struct SpotLight{
	vec3 intensity;
	vec3 position;
	vec3 direction;
	float cutoff;
	float partOfSphere;
} spotLight;

uniform sampler2DMS gPosition;
uniform sampler2DMS gNormal;
uniform sampler2DMS gAmbient;
uniform sampler2DMS gDiffuse;
uniform sampler2DMS gSpecular;
uniform sampler2D ssao;

//uniform vec3 cameraPos;

out vec4 color;

vec3 calcLight() {
	vec3 view_pos = texelFetch(gPosition, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 view_normal = texelFetch(gNormal, ivec2(gl_FragCoord), gl_SampleID).xyz;

	vec3 toLight = spotLight.position - view_pos;
	vec3 direction = normalize(toLight);

	// roughly based on https://www.youtube.com/watch?v=617njAJuU6U
	float spot = dot(direction, -spotLight.direction);
	if (spot <= spotLight.cutoff) return vec3(0,0,0);
	float spotFactor = 1.0 - (1.0 - spot) / (1.0 - spotLight.cutoff);

	vec3 kA = texelFetch(gAmbient, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kD = texelFetch(gDiffuse, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kS = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).xyz;
	float n = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).w;

	float cosTheta = max(0, dot(view_normal, direction));
	vec3 cameraDir = normalize(-view_pos);
	vec3 reflected = reflect(-direction, view_normal);
	float cosAlpha = max(0, dot(cameraDir, reflected));

	vec2 size = textureSize(ssao, 0);
	float ao = texture(ssao, vec2(gl_FragCoord) / size).r;

	vec3 ambient = kA * ao;
	vec3 diffuse = kD * cosTheta;
	vec3 specular = kS * pow(cosAlpha, n);

	vec3 intensity = spotLight.intensity / (spotLight.partOfSphere * dot(toLight, toLight));
	return intensity * pow(spotFactor, 2) * (ambient + diffuse + specular);
}

void main() {
	color = vec4(calcLight(), 1);
	//color = vec4(1,0,0,1);
}
