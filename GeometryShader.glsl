#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 PROJ_MAT;
uniform mat4 VIEW_MAT;
uniform mat4 MODEL_MAT;

in VS_OUT {
	vec3 FragPos[];
	vec2 aTexture[];
	mat3 TBN[];
	vec3 tangentLightPos[];
	vec3 tangentViewPos[];
	vec3 tangentFragPos[];
} gs_in;

out GS_OUT {
	vec3 aFragPos;
	vec2 TextureCoords;
	mat3 matTBN;
	vec3 tLightPos;
	vec3 tViewPos;
	vec3 tFragPos;
} gs_out;

in vec2 aTexture[];
in vec3 normalsOut[];
out vec2 texUVs;
out vec4 fragPos;
out vec3 finalNormals;
out vec3 pointToCamera;

out float diffValue;
out float specValue;

in vec4 shadow_coord[]; // PF
out vec4 final_shadow_coord; // PF

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

			//Pass variables for normal mapping to fragment shader
			gs_out.TextureCoords = gs_in.aTexture[i];
			gs_out.aFragPos = gs_in.FragPos[i];
			gs_out.matTBN = gs_in.TBN[i];
			gs_out.tLightPos = gs_in.tangentLightPos[i];
			gs_out.tFragPos = gs_in.tangentFragPos[i];
			gs_out.tViewPos = gs_in.tangentViewPos[i];

			final_shadow_coord = shadow_coord[i]; // PF
			gl_Position = (PROJ_MAT * VIEW_MAT * MODEL_MAT) * gl_in[i].gl_Position;
			fragPos = MODEL_MAT * gl_in[i].gl_Position; //Position in model space
			pointToCamera = (inverse(VIEW_MAT) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz - fragPos.xyz; //Vector that goes from point to camera
			EmitVertex();
		}
		EndPrimitive();
	}
}