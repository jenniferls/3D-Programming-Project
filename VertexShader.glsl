#version 440
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texture_coords;
layout(location = 2) in vec3 normals;

out vec2 aTexture;
layout(location=3) out vec3 normalsOut;
//out vec4 vertexPos;

void main() {
	aTexture = texture_coords; //Pass the texture info to the geometry shader
	normalsOut = normals;
	// gl_Position = newVertex;//vec4(vec3(newVertex.x + offset, newVertex.yz), 1.0);
	// gl_Position = vec4(vertex_position, 1.0);
	gl_Position = vec4(vertex_position, 1.0);
}