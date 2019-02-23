#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string filePath) {
	this->path = filePath;
	this->vaoID = 0;
	this->vboID = 0;
	this->iboID = 0;
	this->vertCount = 0;
	this->numIndices = 0;
}

AnimatedModel::~AnimatedModel() {

}

std::string AnimatedModel::getPath() const {
	return this->path;
}

void AnimatedModel::setVertCount(unsigned int count) {
	this->vertCount = count;
}

unsigned int AnimatedModel::getVertCount() const {
	return this->vertCount;
}

unsigned int AnimatedModel::getVaoID() const {
	return this->vaoID;
}

void AnimatedModel::setVaoID(unsigned int id) {
	this->vaoID = id;
}

unsigned int AnimatedModel::getVboID() const {
	return this->vboID;
}

void AnimatedModel::setVboID(unsigned int id) {
	this->vboID = id;
}
