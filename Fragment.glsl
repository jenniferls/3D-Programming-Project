#version 440
// these values are interpolated at the rasteriser
in vec2 aTexture;
layout(location=3) in float myAttrOut;

// this is the final pixel colour
out vec4 fragment_color;

// this is a uniform value, the very same value for ALL pixel shader executions
uniform sampler2D texSampler;

void main () {
	float ambientStr = 0.1f;
	vec3 ambientColor = ambientStr * vec3(1.0f);
	vec4 textureSample = vec4 (texture(texSampler, vec2 (1-aTexture.s, 1-aTexture.t)));
	fragment_color = textureSample;
	//fragment_color = vec4 (color * colourFromImGui, 1.0);
	//fragment_color = myAttrOut * vec4 (color * colourFromImGui, 1.0);
}