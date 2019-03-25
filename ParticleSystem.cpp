#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {
	this->particleCount = 0;
}


ParticleSystem::~ParticleSystem() {

}

unsigned int ParticleSystem::getCount() const{
	return this->particleCount;
}