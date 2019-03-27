#include "ParticleSystem.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>

ParticleSystem::ParticleSystem() {
	this->particleCount = 0;
	this->textureID = 0;
	this->vaoID = 0;
	this->vboID = 0;

	addParticle();
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

void ParticleSystem::prepareBuffers() {
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
}

void ParticleSystem::prepare(unsigned int& shaderProg) {
	glBindVertexArray(vaoID);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
}

void ParticleSystem::addParticle() {
	this->particles[0] = Particle();
	this->particleCount++;
}

unsigned int ParticleSystem::getCount() const{
	return this->particleCount;
}

std::string ParticleSystem::getTexPath() const {
	return this->texPath;
}