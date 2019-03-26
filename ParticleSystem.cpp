#include "ParticleSystem.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>

ParticleSystem::ParticleSystem() {
	this->particleCount = 0;
	this->maxParticles = 100;
	this->textureID = 0;
	this->vaoID = 0;
	this->vboID = 0;

	createBuffers();
}


ParticleSystem::~ParticleSystem() {
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
}

void ParticleSystem::createBuffers() {
	glGenVertexArrays(1, &vaoID);

	glGenBuffers(1, &vboID);
}

unsigned int ParticleSystem::getCount() const{
	return this->particleCount;
}