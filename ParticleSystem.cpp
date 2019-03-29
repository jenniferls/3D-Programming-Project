#include "ParticleSystem.h"
#include "windows.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>

ParticleSystem::ParticleSystem() {
	this->particleCount = 0;
	this->textureID = 0;
	this->vaoID = 0;
	this->vboID = 0;
	this->timer = 0.0;
	this->emitRate = 0.1;
	this->testCounter = 0.0;

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_COPY); //Data that will be modified by compute shader

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
}

void ParticleSystem::prepare(unsigned int& shaderProg) {
	glBindVertexArray(vaoID);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
}

void ParticleSystem::addParticle() {
	//this->particles[this->particleCount] = Particle();
	//this->testCounter += 0.2;
	//this->particles[this->particleCount].pos = glm::vec3(this->testCounter, 0.0, 0.0);
	this->particleCount++;
	OutputDebugStringA("Particle added\n");
}

void ParticleSystem::update(double dt) {
	this->timer -= 1.0 * dt;
	if (this->particleCount < MAX_PARTICLES && this->timer <= 0) {
		addParticle();
		this->timer = this->emitRate;
	}
}

unsigned int ParticleSystem::getCount() const{
	return this->particleCount;
}

std::string ParticleSystem::getTexPath() const {
	return this->texPath;
}