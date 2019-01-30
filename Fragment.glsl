#version 440
// these values are interpolated at the rasteriser
in vec2 texUVs;
in vec3 finalNormals;

// this is the final pixel colour
out vec4 fragment_color;

// PF: the diffuse value
in float diffValue;
in float specValue;

// this is a uniform value, the very same value for ALL pixel shader executions
uniform sampler2D textureSampler;

void main () {	
	float amStr = 0.5f; // PF: the ambient strenght 
	float specStr = 0.5f; // PF: the specular strenght
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float MatSpecIntensity = 1.0f;

	vec3 am = amStr * lightColor; // PF: the ambient 
	vec3 diffuse = diffValue * lightColor; // PF: the diffuse
	vec3 spec = specStr * specValue * lightColor;
	
	vec4 texSample = texture( textureSampler, vec2(texUVs.s, 1 - texUVs.t));

	vec4 result = vec4(am + diffuse + spec, 1.0 ) * texSample;
	fragment_color = result;
}