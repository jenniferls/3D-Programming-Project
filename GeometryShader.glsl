#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 PROJ_MAT;
uniform mat4 VIEW_MAT;
uniform mat4 MODEL_MAT;

uniform mat4 MVP_MAT; //Testing

in vec2 aTexture[];
in vec3 normalsOut[];
out vec2 texUVs;
out vec4 fragPos;
out vec3 finalNormals;

out float diffValue;
const vec3 light_pos = vec3( 0.0, 0.0, 2.0);
//in vec4 vertexPos;

float getDiffVal(vec4 normal){
	
	vec4 normalVec = normalize( vec4(light_pos, 1.0));
	float diffuseFactor = dot(normalVec.xyz, normal.xyz); 

	if (diffuseFactor < 0)
		diffuseFactor = 0;
	
	return diffuseFactor * 4.0f * (1.0/(length(vec4(light_pos, 1.0))));
}

//vec4 getNormal(){
//
//	vec3 u = vec3(vertexPos[0]) - vec3(vertexPos[1]);
//	vec3 v = vec3(vertexPos[2]) - vec3(vertexPos[1]);
//
//	return vec4(normalize(cross(u,v)), 0.0f);
//}
//

void main(){
	//First determine the normal of the face by calculating the cross product of two vectors created by 3 triangle vertices
	//This gives us a vector that is perpendicular to the two vectors
	vec3 n = cross(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz); //Order determines if the resulting vector will be positive or negative.
	vec3 faceNormal = mat3(VIEW_MAT * MODEL_MAT) * n; //Make sure the normal is in view space

	vec4 theNormal = vec4( n, 1.0f) * MODEL_MAT;

	//Display triangle if it's facing the camera
	float angle = dot(faceNormal, vec3((PROJ_MAT * VIEW_MAT * MODEL_MAT) * gl_in[0].gl_Position));
	if(angle >= 0.0f){
		for(int i = 0; i < gl_in.length(); i++){
			texUVs = aTexture[i];
			finalNormals = normalsOut[i];
			gl_Position = (PROJ_MAT * VIEW_MAT * MODEL_MAT) * gl_in[i].gl_Position; //Correct order (works if projection and view matrices are transposed first)
			//gl_Position = gl_in[i].gl_Position * (MODEL_MAT * VIEW_MAT * PROJ_MAT); //Reversed order (works if matrices aren't transposed)
			//gl_Position = MVP_MAT * gl_in[i].gl_Position; //Test
			fragPos = MODEL_MAT * gl_in[i].gl_Position; //Position in world space
			diffValue = getDiffVal( theNormal);
			EmitVertex();
		}
		EndPrimitive();
	}
}