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
const vec3 light_pos = vec3( 0.0, 1.0, 3.0);

float getDiffVal(vec4 offset, vec4 normal){
	
	vec4 normalVec = normalize( vec4(light_pos, 1.0));
	float diffuseFactor = dot(normalVec.xyz, normal.xyz); 

	if (diffuseFactor < 0)
		diffuseFactor = 0;
	
	return diffuseFactor * 4.0f * (1.0/(length(vec4(light_pos, 1.0) - offset)));
}

void main(){
	//First determine the normal of the face by calculating the cross product of two vectors created by 3 triangle vertices
	//This gives us a vector that is perpendicular to the two vectors
	vec3 n = cross(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz); //Order determines if the resulting vector will be positive or negative.
	vec3 faceNormal = mat3(VIEW_MAT * MODEL_MAT) * n; //Make sure the normal is in view space

	vec3 n2 = normalize(n);

	vec4 theNormal = MODEL_MAT * vec4( n2, 1.0f);

	//Display triangle if it's facing the camera
	float angle = dot(faceNormal, vec3((VIEW_MAT * MODEL_MAT) * gl_in[0].gl_Position)); //Calculate the dot product between the face normal and camera view (point is in view space)
	if(angle <= 0.0f){
		for(int i = 0; i < gl_in.length(); i++){
			texUVs = aTexture[i];
			finalNormals = normalsOut[i];
			gl_Position = (PROJ_MAT * VIEW_MAT * MODEL_MAT) * gl_in[i].gl_Position;
			fragPos = MODEL_MAT * gl_in[i].gl_Position; //Position in world space
			diffValue = getDiffVal(-fragPos, theNormal);
			EmitVertex();
		}
		EndPrimitive();
	}
}