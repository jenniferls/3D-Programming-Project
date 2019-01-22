#include "RawModel.h"

RawModel::RawModel(int id, int vertCount) {
	this->id = id;
	this->vertCount = vertCount;
}

RawModel::~RawModel() {

}

int RawModel::getID() const {
	return this->id;
}

int RawModel::getvertCount() const {
	return this->vertCount;
}
