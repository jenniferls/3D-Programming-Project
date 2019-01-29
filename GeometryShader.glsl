#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 PROJ_MAT;
uniform mat4 VIEW_MAT;
uniform mat4 MODEL_MAT;

in vec2 aTexture[];
in vec3 normalsOut[];
out vec2 texUVs;
out vec4 fragPos;
out vec3 finalNormals;

out float diffValue;
out float specValue;
const vec3 light_pos = vec3( 2.0, 1.5, 3.0);

float getDiffVal(vec4 fragPos, vec3 normal){
	
	vec3 pointToLight = normalize(light_pos - fragPos.xyz);

	float diffuseFactor = dot(pointToLight, normal) / (length(pointToLight)) * length(normal);

	if (diffuseFactor < 0)
		diffuseFactor = 0;
	
	return diffuseFactor;
}

float getSpecVal(vec4 fragPos, vec3 normal){
	
	//http://ogldev.atspace.co.uk/www/tutorial19/tutorial19.html
	vec3 camPos = vec3(0.0f, 0.0f, 2.0f); // <--- temp 

	vec3 lightVec = normalize(light_pos - fragPos.xyz);
	vec3 viewVec = normalize(fragPos.xyz - camPos);

	vec3 reflection = lightVec - 2 * normal * (dot(normal, lightVec)); // 1.
	vec3 reflectVec = normalize(reflect(lightVec, normal)); // 2. the reflect funktion does the same as the above in 1. 

	float specularFactor = dot(viewVec, reflectVec);

	float specularPower = 32.0f;
	if(specularFactor > 0){
		specularFactor = pow(specularFactor, specularPower); // <- specularPower is how reflective a material is 
	}

	return specularFactor;
}

void main(){
	//First determine the normal of the face by calculating the cross product of two vectors created by 3 triangle vertices
	//This gives us a vector that is perpendicular to the two vectors
	vec3 n = cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz); //Order determines if the resulting vector will be positive or negative.
	vec3 faceNormal = mat3(VIEW_MAT * MODEL_MAT) * n; //Make sure the normal is in view space

	//Display triangle if it's facing the camera
	float angle = dot(faceNormal, vec3((VIEW_MAT * MODEL_MAT) * gl_in[0].gl_Position)); //Calculate the dot product between the face normal and camera view (point is in view space)
	if(angle <= 0.0f){
		for(int i = 0; i < gl_in.length(); i++){
			texUVs = aTexture[i];
			finalNormals = normalsOut[i];
			gl_Position = (PROJ_MAT * VIEW_MAT * MODEL_MAT) * gl_in[i].gl_Position;
			fragPos = MODEL_MAT * gl_in[i].gl_Position; //Position in world space
			diffValue = getDiffVal(fragPos, normalize(normalsOut[i]));
			specValue = getSpecVal( fragPos, normalize(normalsOut[i]));
			EmitVertex();
		}
		EndPrimitive();
	}
}