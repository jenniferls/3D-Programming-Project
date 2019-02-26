#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texture_coords_SM;
layout(location = 2) in vec3 normals_SM;

uniform mat4 SHADOW_MAT;

void main() {

	gl_Position = SHADOW_MAT * vec4(vertex_position, 1.0);
}
