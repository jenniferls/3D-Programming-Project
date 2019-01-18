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

void main(){
	//First determine the normal of the face by calculating the cross product of two vectors created by 3 triangle vertices
	//This gives us a vector that is perpendicular to the two vectors
	vec3 n = cross(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz); //Order determines if the resulting vector will be positive or negative.
	vec3 faceNormal = mat3(VIEW_MAT * MODEL_MAT) * n; //Make sure the normal is in view space

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
			EmitVertex();
		}
		EndPrimitive();
	}
}