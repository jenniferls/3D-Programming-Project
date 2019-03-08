#include "Scene.h"

Scene::Scene() {
	this->modelCount = 0;
	this->animatedModelCount = 0;
	this->lightCount = 0;
}


Scene::~Scene() {
	OutputDebugStringA("Destructor is run for scene\n");
	for (int i = 0; i < this->animatedModelCount; i++) {
		delete animatedModels[i];
	}
	animatedModels.clear();
}

void Scene::addModel(const char* path) {
	RawModel model(path);
	loader.loadOBJ(model); //Loads model from file
	loader.loadMTL(model); //Loads material from file
	models.push_back(model);
	this->modelCount++;
}

void Scene::addBlendmapModel(const char* path) {
	RawModel model(path);
	loader.loadOBJ(model); //Loads model from file
	loader.loadMTL(model); //Loads material from file
	blendmapModels.push_back(model);
	this->blendMapModelCount++;
}

void Scene::addAnimatedModel(std::string path) {
	animatedModels.push_back(new AnimatedModel(path));
	animLoader.LoadModel(animatedModels[this->animatedModelCount]);//Load the model
	this->animatedModelCount++;
}

int Scene::getModelCount() const {
	return this->modelCount;
}

unsigned int Scene::getAnimModelCount() const {
	return this->animatedModelCount;
}

int Scene::getBlendmapModelCount() const {
	return this->blendMapModelCount;
}

void Scene::addLight(glm::vec3 position, glm::vec3 color) {
	Light light(position, color); //Initialize with position and color
	lights.push_back(light);
	lightPositions.push_back(position);
	lightColors.push_back(color);
	this->lightCount++;
}

int Scene::getLightCount() const {
	return this->lightCount;
}
