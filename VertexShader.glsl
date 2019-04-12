#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texture_coords;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 aTexture;
out vec3 normalsOut;
out vec4 shadow_coord;
out vec3 viewPos;


out vec3 FragPos;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 MODEL_MAT; // PF
uniform mat4 SHADOW_MAT; // PF
uniform mat4 VIEW_MAT;

vec3 lightPos = vec3(4.0, 6.0, 2.0);

void main() {
	aTexture = texture_coords; //Pass the texture info to the geometry shader
	normalsOut = normals;
	FragPos = vec3(MODEL_MAT * vec4(vertex_position, 1.0));
	viewPos = (inverse(VIEW_MAT)* vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz - FragPos;

	mat3 normalMat = transpose(inverse(mat3(MODEL_MAT)));
	vec3 t = normalize(normalMat * aTangent);
	vec3 n = normalize(normalMat * aBitangent);
	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(n, t);

	mat3 tbnMatrix = transpose(mat3(t, b, n));
	TangentLightPos = tbnMatrix * lightPos;
	TangentViewPos = tbnMatrix * viewPos;
	TangentFragPos = tbnMatrix * FragPos;

	gl_Position = vec4(vertex_position, 1.0);

	shadow_coord = SHADOW_MAT * (MODEL_MAT * vec4(vertex_position, 1.0)); // PF
}