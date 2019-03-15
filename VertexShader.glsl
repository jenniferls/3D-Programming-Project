#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texture_coords;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 aTexture;
out vec3 normalsOut;
out vec4 shadow_coord;

out VS_OUT {
	vec3 FragPos;
	vec2 aTexture;
	mat3 TBN;
	vec3 tangentLightPos;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
} vs_out;


uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 PROJ_MAT;
uniform mat4 VIEW_MAT; 
uniform mat4 MODEL_MAT; // PF
uniform mat4 SHADOW_MAT; // PF

void main() {
	vs_out.aTexture = texture_coords; //Pass the texture info to the geometry shader
	normalsOut = normals;

	//Normal calculations
	vec3 T = normalize(vec3(MODEL_MAT * vec4(aTangent, 0.0)));
	vec3 N = nomralize(vec3(MODEL_MAT * vec4(normals, 0.0)));
	vec3 B = cross (N, T);

	mat3 TBN = transpose(mat3(T,B,N));
	vs_out.tangentLightPos = TBN * lightPos;
	vs_out.tangentViewPos = TBN * viewPos;
	vs_out.tangentFragPos = TBN * vec3(MODEL_MAT * vec4(vertex_position, 0.0));

	gl_Position = vec4(vertex_position, 1.0);

	shadow_coord = SHADOW_MAT * (MODEL_MAT * vec4(vertex_position, 1.0)); // PF
}