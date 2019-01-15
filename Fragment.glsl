#version 440
// these values are interpolated at the rasteriser
in vec2 aTexture;
//in vec2 texUVs; //For use with geometry shader
layout(location=3) in float myAttrOut;

// this is the final pixel colour
out vec4 fragment_color;

// this is a uniform value, the very same value for ALL pixel shader executions
uniform sampler2D textureSampler;
void main () {
	fragment_color = texture(textureSampler, aTexture);
	//fragment_color = texture(textureSampler, texUVs); //For use with geometry shader;

	//fragment_color = vec4 (color * colourFromImGui, 1.0);
	//fragment_color = myAttrOut * vec4 (color * colourFromImGui, 1.0);
}