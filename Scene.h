#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "RawModel.h"
#include "AnimatedModel.h"
#include "OBJLoader.h"
#include "AssimpLoader.h"
#include "Light.h"
#include "Skybox.h"

class Scene {
public:
	Scene();
	~Scene();

	void addModel(const char* path);
	void addAnimatedModel(std::string path);
	int getModelCount() const;
	unsigned int getAnimModelCount() const;

	void addLight(glm::vec3 position, glm::vec3 color);
	int getLightCount() const;

	std::vector<RawModel> models;
	std::vector<AnimatedModel*> animatedModels;

	std::vector<Light> lights; //List of lights
	std::vector<glm::vec3> lightPositions; //List of all light positions
	std::vector<glm::vec3> lightColors; //List of all light colors

	AssimpLoader animLoader;
	Skybox skybox = "Resources/Textures/Skybox1";

private:
	OBJLoader loader;
	int modelCount;
	unsigned int animatedModelCount;
	int lightCount;
};