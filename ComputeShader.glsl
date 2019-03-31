#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_shading_language_include : require


layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in; //Threads

struct Particle {
	vec4 pos;
	vec4 velocity;
//	vec4 color;
//	float lifetime;
//	float acceleration;
	int isDead;
};
layout (std430, binding = 0) buffer particles {
	Particle particleData[];
};

uniform float dt;

void main(){

uint index = gl_GlobalInvocationID.x;
	if(particleData[index].isDead == 0){ //If the particle is not marked as dead
		particleData[index].pos -= particleData[index].velocity * dt;
	}
}