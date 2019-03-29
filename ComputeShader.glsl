#version 440
#extension GL_ARB_compute_shader : enable

layout (local_size_x = 10, local_size_y = 10) in; //Threads

struct Particle {
	vec3 pos;
//	vec3 velocity;
//	float lifetime;
//	float acceleration;
//	bool isDead;
};
layout (std430, binding = 0) buffer particles {
	Particle particleData[];
};

uniform float dt;

const int MAX_PARTICLES = 100; //Needs to match max particles on CPU

void main(){
//	int i = 0;
//	while(i < MAX_PARTICLES){
//		if(particlesData[i].isDead == true){ //If particle is dead, reset its' position
//			particlesData[i].pos = vec3(0.0, 0.0, 0.0);
//		}
//		particlesData[i].pos -= vec3(0.0, 1.0, 0.0) * dt;
//		i++;
//	}

//particlesData[0].pos -= vec3(0.0, 1.0, 0.0) * dt;
//particlesData[1].pos += vec3(0.0, 1.0, 0.0) * dt;

uint index = gl_GlobalInvocationID.x;
Particle p = particleData[index];
p.pos -= vec3(0.0, 1.0, 0.0) * dt;

particleData[index] = p; //Copy data back
}