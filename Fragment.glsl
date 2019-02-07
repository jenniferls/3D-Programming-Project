#version 440
// these values are interpolated at the rasteriser
in vec2 texUVs;
in vec3 finalNormals;

// this is the final pixel colour
out vec4 fragment_color;

// PF: the diffuse value
//in float diffValue;
//in float specValue;

// this is a uniform value, the very same value for ALL pixel shader executions
uniform sampler2D textureSampler;
uniform mat4 MODEL_MAT;
uniform vec3 light_positions[];
uniform vec3 light_colors[];

in vec4 fragPos;
in vec3 lightToCamera;

void main () {	
	float amStr = 0.2f; // PF: the ambient strength 
	float specStr = 0.5f; // PF: the specular strength
	float MatSpecIntensity = 1.0f;

	vec3 am = amStr * light_colors[0]; // PF: the ambient 
//	vec3 diffuse = diffValue * lightColor; // PF: the diffuse
//	vec3 spec = specStr * specValue * lightColor;
	
	vec4 texSample = texture(textureSampler, vec2(texUVs.s, 1 - texUVs.t)); //Texture

	//Diffuse shading
	vec3 pointToLight = normalize(light_positions[0] - fragPos.xyz);
	vec3 norm = normalize(mat3(MODEL_MAT) * finalNormals); //Make sure the vectors are normalized in world space
	float diffuseFactor = dot(pointToLight, norm) / (length(pointToLight) * length(norm));
	diffuseFactor = clamp(diffuseFactor, 0, 1); //Make sure the diffuse factor isn't negative or above 1
	vec3 diffuse = diffuseFactor * light_colors[0];

	//Specularity
	vec3 unitLightToCam = normalize(lightToCamera);
	vec3 lightDir = -pointToLight;
	vec3 reflectedLightDir = reflect(lightDir, norm);
	float specFactor = dot(reflectedLightDir, unitLightToCam);
	specFactor = clamp(specFactor, 0, 1); //Make sure the specular factor isn't negative or above 1
	float shineDamper = 32f;
	float reflectivity = 0.8f;
	float dampedSpec = pow(specFactor, shineDamper);
	vec3 specular = dampedSpec * reflectivity * light_colors[0];

	vec4 result = vec4(am + diffuse, 1.0 ) * texSample + vec4(specular, 1.0f);
	fragment_color = result;
}