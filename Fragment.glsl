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

const vec3 light_pos = vec3( 4.0, 0.0, 2.0);
in vec4 fragPos;
uniform mat4 MODEL_MAT;

void main () {	
	float amStr = 0.5f; // PF: the ambient strenght 
	float specStr = 0.5f; // PF: the specular strenght
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float MatSpecIntensity = 1.0f;

	vec3 am = amStr * lightColor; // PF: the ambient 
//	vec3 diffuse = diffValue * lightColor; // PF: the diffuse
	vec3 spec = specStr * specValue * lightColor;
	
	vec4 texSample = texture( textureSampler, vec2(texUVs.s, 1 - texUVs.t));

	vec3 pointToLight = normalize(light_pos - fragPos.xyz);
	vec3 norm = normalize(mat3(MODEL_MAT) * finalNormals); //Make sure the vectors are normalized in world space
	float diffuseFactor = dot(pointToLight, norm) / (length(pointToLight) * length(norm));
	diffuseFactor = clamp(diffuseFactor, 0, 1); //Make sure the diffuse factor isn't negative or above 1
	vec3 diffuse = diffuseFactor * lightColor;


	vec4 result = vec4(am + diffuse, 1.0 ) * texSample;
	fragment_color = result;
}