#version 440
// these values are interpolated at the rasteriser
in vec2 texUVs;
layout(location=3) in vec3 finalNormals;

// this is the final pixel colour
out vec4 fragment_color;

// PF: the diffuse value
in float diffValue;

// this is a uniform value, the very same value for ALL pixel shader executions
uniform sampler2D textureSampler;

uniform vec3 viewPos;

void main () {	
	float amStr = 0.1f; // PF: the ambient strenght 
	float specStr = 0.5f; // PF: the specular strenght

	vec3 am = amStr * vec3(1.0, 1.0, 1.0); // PF: the ambient 
	
	//vec4 texSample = texture( textureSampler, vec2( 1 - texUVs.s, 1 - texUVs.t)) * diffValue;
	//vec4 applyAm = texture( textureSampler, vec2( 1 - texUVs.s, 1 - texUVs.t)) * vec4(am, 1.0f);

	vec4 texSample = texture( textureSampler, texUVs) * diffValue;
	vec4 applyAm = texture(textureSampler, texUVs) * vec4(am, 1.0f);


	fragment_color = applyAm; 
	fragment_color += texSample;


	//fragment_color = texture(textureSampler, texUVs);
}