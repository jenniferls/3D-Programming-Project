#pragma once
#include <vector>
#include "glm/glm.hpp"

class ParticleSystem {
public:
	struct Particle {
		glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);
		float size = 0.5f;
		float lifetime = 3.0f;
		glm::vec3 velocity = glm::vec3(0.0, 0.0, 0.0);
		float acceleration = 0.0f;
		bool isDead = false;
	};

	ParticleSystem();
	~ParticleSystem();

	void createBuffers();
	unsigned int getCount() const; //Particle count

	std::vector<Particle> particles;
	unsigned int vaoID;
	unsigned int vboID;
	unsigned int textureID;
	std::string texPath = "Resources/Textures/particle.png";

private:
	unsigned int particleCount;
	unsigned int maxParticles;
};

