#include "Scene.h"

Scene::Scene(unsigned int shaderProg) {
	this->modelCount = 0;
	this->lightCount = 0;
	this->shaderProg = shaderProg;
	this->lights_pos_id = -1;
	this->lights_color_id = -1;
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

void Scene::addLight(glm::vec3 position, glm::vec3 color) {
	Light light(position, color); //Initialize with position and color
	lights.push_back(light);
	lightPositions.push_back(position);
	lightColors.push_back(color);
	this->lightCount++;
}

void Scene::prepareLights() {
	//Send arrays of light information to shader
	lights_pos_id = glGetUniformLocation(this->shaderProg, "light_positions"); //Assign ID
	if (lights_pos_id == -1) {
		OutputDebugStringA("Error, cannot find 'lightpos_id' attribute in Fragment shader\n");
	}

	lights_color_id = glGetUniformLocation(this->shaderProg, "light_colors"); //Assign ID
	if (lights_color_id == -1) {
		OutputDebugStringA("Error, cannot find 'lightcolor_id' attribute in Fragment shader\n");
	}
}

int Scene::getLightCount() const {
	return this->lightCount;
}
