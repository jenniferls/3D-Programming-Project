#version 440
#extension GL_ARB_compute_shader : enable

layout (local_size_x = 20, local_size_y = 20) in; //Threads

layout (std430, binding = 0) buffer pos {
	vec4 positions[];
};

void main(){
	
}