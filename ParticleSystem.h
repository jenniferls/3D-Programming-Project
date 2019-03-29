#pragma once
#include <vector>
#include "glm/glm.hpp"

const int MAX_PARTICLES = 100;

class ParticleSystem {
public:
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
	struct Particle {
		glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);
		float pad;
		//glm::vec3 velocity = glm::vec3(0.0, -1.0, 0.0);
		//float pad2;
		//float pos_x = 0.0, pos_y = 0.0, pos_z = 0.0;
		//float vel_x = 0.0, vel_y = 1.0, vel_z = 0.0;
		//float lifetime = 3.0f;
		//float acceleration = 0.0f;
		//bool isDead = false;
	};

	ParticleSystem();
	~ParticleSystem();

	void createBuffers();
	void prepareBuffers();
	void prepare(unsigned int& shaderProg); //Prepare for render

	void addParticle(); //Basically only for debug
	void update(double dt);

	unsigned int getCount() const; //Particle count
	std::string getTexPath() const;

	Particle particles[MAX_PARTICLES];
	unsigned int vaoID;
	unsigned int vboID;
	unsigned int textureID;

private:
	unsigned int particleCount;
	std::string texPath = "Resources/Textures/particle.png";
	float timer;
	float emitRate;
	float testCounter; // Test
};

