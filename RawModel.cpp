#include "RawModel.h"

RawModel::RawModel(int id, const char* filePath) {
	this->id = id;
	this->vertCount = 0;
	this->path = filePath;
	this->texturePath = "Path not loaded";
	this->textureID = 0;
	this->ambientVal = { 0.0f, 0.0f, 0.0f };
	this->diffuseVal = { 0.0f, 0.0f, 0.0f };
	this->specularVal = { 0.0f, 0.0f, 0.0f };
	this->materialPath = "Path not loaded";
}

RawModel::~RawModel() {

}

int RawModel::getID() const {
	return this->id;
}

int RawModel::getVertCount() const {
	return this->vertCount;
}

void RawModel::setVertCount(int count) {
	this->vertCount = count;
}

void RawModel::setTextureID(unsigned int id) {
	this->textureID = id;
}

const char* RawModel::getPath() const {
	return this->path;
}

std::string RawModel::getTexturePath() const {
	return this->texturePath;
}

void RawModel::setTexturePath(std::string texPath) {
	this->texturePath = texPath;
}

void RawModel::setMaterialPath(std::string matPath) {
	this->materialPath = matPath;
}

std::string RawModel::getMaterialPath() const {
	return this->materialPath;
}

unsigned int RawModel::getTextureID() const {
	return this->textureID;
}
