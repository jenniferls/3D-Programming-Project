#version 440
layout(location = 0) in vec3 pos;

uniform mat4 VIEW_MAT;
uniform mat4 MODEL_MAT;

void main(){
	gl_Position = (VIEW_MAT * MODEL_MAT) * vec4(pos, 1.0);
}