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

void Scene::addModel(const char* path, unsigned int& shaderProg) {
	RawModel model(path, shaderProg);
	loader.loadOBJ(model); //Loads model from file
	loader.loadMTL(model); //Loads material from file
	models.push_back(model);
	this->modelCount++;
}

void Scene::addAnimatedModel(std::string path, unsigned int& shaderProg) {
	animatedModels.push_back(new AnimatedModel(path, shaderProg));
	animLoader.LoadModel(animatedModels[this->animatedModelCount]);//Load the model
	this->animatedModelCount++;
}

int Scene::getModelCount() const {
	return this->modelCount;
}

unsigned int Scene::getAnimModelCount() const {
	return this->animatedModelCount;
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
