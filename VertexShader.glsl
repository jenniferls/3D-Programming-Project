#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in float myAttr;

out vec3 color;
layout(location=3) out float myAttrOut;

// uniform offset
layout(location=10) uniform float offset;

layout(location=11) uniform mat4 rotateZ;
void main() {
	color = vertex_color;
	myAttrOut = myAttr;
	// gl_Position = newVertex;//vec4(vec3(newVertex.x + offset, newVertex.yz), 1.0);
	// gl_Position = vec4(vertex_position, 1.0);

 	vec4 newVertex = rotateZ * vec4(vertex_position, 1.0f);
 	gl_Position = vec4(vec3(newVertex.x + offset, newVertex.yz), 1.0);
}