#pragma once
#include <vector>
#include <string>
#include "RawModel.h"
#include "OBJLoader.h"

class Scene {
public:
	Scene();
	~Scene();

	void addModel(int id, const char* path, const char* texPath);
	int getModelCount() const;

	std::vector<RawModel> models;
private:
	OBJLoader loader;
	int modelCount;
};

