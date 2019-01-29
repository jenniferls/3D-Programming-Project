#include "RawModel.h"

RawModel::RawModel(int id, int vertCount, std::string filePath) {
	this->id = id;
	this->vertCount = vertCount;
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
