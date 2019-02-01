#include "Scene.h"

Scene::Scene() {
	this->modelCount = 0;
}


Scene::~Scene() {

}

void Scene::addModel(int id, const char* path, const char* texPath) {
	RawModel model(id, path, texPath);
	loader.loadOBJ(model);
	models.push_back(model);
	this->modelCount++;
}

int Scene::getModelCount() const {
	return this->modelCount;
}
