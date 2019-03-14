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

in vec4 final_shadow_coord;
uniform sampler2D shadowMap;

float shadowCalc(vec4 shadow_coord){
	
	vec3 proj_coord = shadow_coord.xyz/shadow_coord.w;
	proj_coord = proj_coord * 0.5 + 0.5;
	float closetsDepth = texture(shadowMap, proj_coord.xy).r;
	float currentDepth = proj_coord.z;
	float bias = 0.005;
	float shadow = currentDepth - bias > closetsDepth ? 1.0 : 0.0;

	return shadow;
}

vec4 calcDiffuse(vec3 light_pos, vec3 light_color, vec3 normal){
//	vec3 am = ambient_val * light_color; // PF: the ambient 
	//Diffuse shading
	vec3 pointToLight = normalize(light_pos - fragPos.xyz);
	float diffuseFactor = dot(pointToLight, normal) / (length(pointToLight) * length(normal));
	diffuseFactor = clamp(diffuseFactor, 0, 1); //Make sure the diffuse factor isn't negative or above 1
	vec3 diffuse = diffuseFactor * light_color;
	vec4 final = vec4(diffuse, 1.0 );
	return final;
}

vec4 calcSpecular(vec3 light_pos, vec3 light_color, vec3 normal){
	//Specularity
	vec3 unitLightToCam = normalize(lightToCamera);
	vec3 lightDir = -normalize(light_pos - fragPos.xyz);
	vec3 reflectedLightDir = reflect(lightDir, normal);
	float specFactor = dot(reflectedLightDir, unitLightToCam);
	specFactor = clamp(specFactor, 0, 1); //Make sure the specular factor isn't negative or above 1
	float shineDamper = 32f;
	float reflectivity = 0.8f;
	float dampedSpec = pow(specFactor, shineDamper);
	vec3 specular = dampedSpec * reflectivity * specular_val * light_color;
	vec4 final = vec4(specular, 1.0f);
	return final;
}

vec4 calcAmbient(vec3 light_color){
	vec3 amb = ambient_val * light_color;
	return vec4(amb, 1.0);
}

void main () {
	vec4 texSample = texture(textureSampler, vec2(texUVs.s, 1 - texUVs.t)); //Texture

	vec3 norm = normalize(mat3(MODEL_MAT) * finalNormals); //Make sure the vectors are normalized in world space

	float shadow = shadowCalc(final_shadow_coord);

	vec4 result = vec4(0.0f);
//	result += calcDiffuse(light_positions[0], light_colors[0], norm) * texSample + calcSpecular(light_positions[0], light_colors[0], norm);
//	result += calcDiffuse(light_positions[1], light_colors[1], norm) * texSample + calcSpecular(light_positions[1], light_colors[1], norm);

	result += (calcAmbient(light_colors[0]) + (1.0 - shadowCalc(final_shadow_coord)) * calcDiffuse(light_positions[0], light_colors[0], norm)) * (texSample + calcSpecular(light_positions[0], light_colors[0], norm));
//	result += (calcAmbient(light_colors[1]) + (1.0 - shadowCalc(final_shadow_coord)) * calcDiffuse(light_positions[1], light_colors[1], norm)) * texSample + calcSpecular(light_positions[1], light_colors[1], norm);


//	vec3 am = ambient_val * light_colors[0];
//	vec3 pointToLight = normalize(light_positions[0] - fragPos.xyz);
//	float diff = max(dot(pointToLight, norm), 0.0);
//	vec3 diffuse = diff * light_colors[0];

//	result += (vec4(am, 1.0) + (1.0 - shadow)) * (vec4(diffuse, 1.0) * texSample + calcSpecular(light_positions[1], light_colors[1], norm));

	fragment_color = result;
}