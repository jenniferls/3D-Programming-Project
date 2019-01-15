#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 PROJ_MAT;
uniform mat4 VIEW_MAT;
uniform mat4 MODEL_MAT;

in vec2 aTexture[];
out vec2 texUVs;
out vec4 fragPos;

void main(){
	//Input primitive
	for(int i = 0; i < gl_in.length(); i++){
		texUVs = aTexture[i];
		gl_Position = gl_in[i].gl_Position * MODEL_MAT * VIEW_MAT * PROJ_MAT; //Reversed order
		fragPos = MODEL_MAT * gl_in[i].gl_Position; //Position in world space
		EmitVertex();
	}
	EndPrimitive();
}