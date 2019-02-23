#include "AnimatedModel.h"

AnimatedModel::AnimatedModel(std::string filePath) {
	this->path = filePath;
	this->texturePath = "Path not loaded";
	this->vaoID = 0;
	this->vboID = 0;
	this->iboID = 0;
	this->ambID = 0;
	this->diffID = 0;
	this->specID = 0;
	this->vertCount = 0;
	this->numIndices = 0;
	this->worldPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->worldRotation = 0.0f;
	this->ambientVal = { 0.0f, 0.0f, 0.0f };
	this->diffuseVal = { 0.0f, 0.0f, 0.0f };
	this->specularVal = { 0.0f, 0.0f, 0.0f };
}

AnimatedModel::~AnimatedModel() {

}

std::string AnimatedModel::getPath() const {
	return this->path;
}

std::string AnimatedModel::getTexturePath() const {
	return this->texturePath;
}

void AnimatedModel::setTexturePath(std::string texPath) {
	this->texturePath = texPath;
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

unsigned int AnimatedModel::getIboID() const {
	return this->iboID;
}

void AnimatedModel::setIboID(unsigned int id) {
	this->iboID = id;
}

unsigned int AnimatedModel::getTextureID() const {
	return this->textureID;
}

void AnimatedModel::setTextureID(unsigned int id) {
	this->textureID = id;
}

glm::vec3 AnimatedModel::getWorldPosition() const {
	return this->worldPosition;
}

void AnimatedModel::setWorldPosition(glm::vec3 position) {
	this->worldPosition = position;
}

float AnimatedModel::getWorldRotation() const {
	return this->worldRotation;
}

void AnimatedModel::setWorldRotation(float rotation) {
	this->worldRotation = rotation;
}
