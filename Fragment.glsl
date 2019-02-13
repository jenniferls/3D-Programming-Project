#version 440
// these values are interpolated at the rasteriser
in vec2 texUVs;
in vec3 finalNormals;

// this is the final pixel colour
out vec4 fragment_color;

// this is a uniform value, the very same value for ALL pixel shader executions
uniform sampler2D textureSampler;
uniform mat4 MODEL_MAT;
uniform vec3 light_positions[];
uniform vec3 light_colors[];

uniform vec3 ambient_val;	//Ambient color
uniform vec3 diffuse_val;	//Diffuse color (Not used because of texture)
uniform vec3 specular_val;	//Specular color

in vec4 fragPos;
in vec3 lightToCamera;
in vec4 shadow_coord;
uniform sampler2D shadowMap;

float shadowCalc(vec4 shadow_coord){
	
	vec3 proj_coord = shadow_coord.xyz/shadow_coord.w;
	float closetsDepth = texture(shadowMap, proj_coord.xy).r;
	float currentDepth = proj_coord.z;
	float shadow = currentDepth > closetsDepth ? 1.0 : 0.0;

	return shadow;
}

void main () {
	vec3 am = ambient_val * light_colors[0]; // PF: the ambient 
	
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
	vec3 specular = dampedSpec * reflectivity * specular_val * light_colors[0];

	float shadow = shadowCalc(shadow_coord);

	vec4 result = vec4(am + diffuse, 1.0 ) * texSample + vec4(specular, 1.0f);
	fragment_color = result;
}