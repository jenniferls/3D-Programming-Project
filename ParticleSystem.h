#pragma once
#include <vector>
#include "glm/glm.hpp"

class ParticleSystem {
public:
	struct Particle {
		glm::vec3 pos;
	};

	ParticleSystem();
	~ParticleSystem();

	unsigned int getCount() const;

	std::vector<Particle> particles;

private:
	unsigned int particleCount;
};

