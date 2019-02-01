#include "RawModel.h"

RawModel::RawModel(int id, std::string filePath) {
	this->id = id;
	this->vertCount = 0;
	this->path = filePath.c_str();
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

const char * RawModel::getPath() const {
	return this->path;
}
