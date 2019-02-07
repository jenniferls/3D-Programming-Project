#pragma once
#include <vector>
#include <string>
#include "RawModel.h"
#include "OBJLoader.h"
#include "Light.h"

class Scene {
public:
	Scene();
	~Scene();

	void addModel(int id, const char* path, const char* texPath);
	int getModelCount() const;

	void addLight(glm::vec3 position, glm::vec3 color);
	int getLightCount() const;

	std::vector<RawModel> models;
	std::vector<Light> lights;
private:
	OBJLoader loader;
	int modelCount;
	int lightCount;
};

