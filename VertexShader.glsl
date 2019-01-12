#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in float myAttr;

uniform mat4 PROJ_MAT;
uniform mat4 VIEW_MAT;
uniform mat4 MODEL_MAT;

out vec3 color;
layout(location=3) out float myAttrOut;

void main() {
	color = vertex_color;
	myAttrOut = myAttr;
	// gl_Position = newVertex;//vec4(vec3(newVertex.x + offset, newVertex.yz), 1.0);
	// gl_Position = vec4(vertex_position, 1.0);

	gl_Position = vec4(vertex_position, 1.0) * MODEL_MAT * VIEW_MAT * PROJ_MAT; //Reversed order
}