#version 400 core

in struct PointLight{
	vec3 intensity;
	vec3 position;
	vec3 attenuation;
} pointLight;

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

	vec3 kA = texelFetch(gAmbient, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kD = texelFetch(gDiffuse, ivec2(gl_FragCoord), gl_SampleID).xyz;
	vec3 kS = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).xyz;
	float n = texelFetch(gSpecular, ivec2(gl_FragCoord), gl_SampleID).w;

	vec3 direction = normalize(pointLight.position - view_pos);
	float cosTheta = max(0, dot(view_normal, direction));
	vec3 cameraDir = normalize(-view_pos);
	vec3 reflected = reflect(-direction, view_normal);
	float cosAlpha = max(0, dot(cameraDir, reflected));

	vec2 size = textureSize(ssao, 0);
	float ao = texture(ssao, vec2(gl_FragCoord) / size).r;

	vec3 ambient = kA * ao;
	vec3 diffuse = kD * cosTheta;
	vec3 specular = kS * pow(cosAlpha, n);
	
	vec3 toLight = pointLight.position - view_pos;
	float d = length(toLight);

	vec3 atten = pointLight.attenuation;
	vec3 intensity = pointLight.intensity / (atten.x + atten.y * d + atten.z * d * d);
	return intensity * (ambient + diffuse + specular);
}

void main() {
	color = vec4(calcLight(), 1);
}
