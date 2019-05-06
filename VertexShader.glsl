#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texture_coords;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 aTangent;
//layout(location = 4) in vec3 aBitangent;

out vec2 aTexture;
out vec3 normalsOut;
out vec4 shadow_coord;
out vec3 viewPos;
out vec3 tangent;
out vec3 bitangent;

uniform mat4 MODEL_MAT; // PF
uniform mat4 SHADOW_MAT; // PF
uniform mat4 VIEW_MAT;
uniform mat4 PROJ_MAT;

vec3 lightPos = vec3(4.0, 6.0, 2.0);

void main() {
	aTexture = texture_coords; //Pass the texture info to the geometry shader
	
	tangent = vec3(MODEL_MAT * vec4(aTangent, 1.0f));

//	bitangent = vec3(MODEL_MAT * vec4(aBitangent, 1.0f));

	normalsOut = vec3(vec4(normals, 1.0f));

	gl_Position = vec4(vertex_position, 1.0);

	shadow_coord = SHADOW_MAT * (MODEL_MAT * vec4(vertex_position, 1.0)); // PF
}