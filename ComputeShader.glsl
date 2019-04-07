#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable


layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in; //Threads

struct Particle {
	vec4 pos;
	vec4 startPos;
	vec4 direction;
//	vec4 color;
	float lifetime;
	float speed;
};
layout (std430, binding = 0) buffer particles {
	Particle particleData[];
};

uniform float dt;
uniform float max_lifetime;

void InitParticle(uint index){
		particleData[index].pos = particleData[index].startPos;
}

void UpdateParticle(uint index){
	particleData[index].pos -= (particleData[index].direction * particleData[index].speed) * dt;
}

void main(){
	uint index = gl_GlobalInvocationID.x;

	if(particleData[index].lifetime <= 0.0){ //Should probably be exchanged for a lifetime variable
		particleData[index].lifetime = max_lifetime;
		InitParticle(index);
	}

	particleData[index].lifetime -= 1.0 * dt;
	UpdateParticle(index);
}