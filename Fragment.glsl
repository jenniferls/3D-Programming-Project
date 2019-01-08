#version 440
// these values are interpolated at the rasteriser
in vec3 color;
layout(location=3) in float myAttrOut;

// this is the final pixel colour
out vec4 fragment_color;

// this is a uniform value, the very same value for ALL pixel shader executions
layout(location = 5) uniform vec3 colourFromImGui;

void main () {
	fragment_color = vec4 (color, 1.0);
	//fragment_color = vec4 (color * colourFromImGui, 1.0);
	//fragment_color = myAttrOut * vec4 (color * colourFromImGui, 1.0);
}