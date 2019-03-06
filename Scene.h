#pragma once
#include <windows.h>
#include "glew/include/GL/glew.h"
#include <gl/GL.h>

#include <vector>
#include <string>
#include "RawModel.h"
#include "AnimatedModel.h"
#include "OBJLoader.h"
#include "AssimpLoader.h"
#include "Light.h"

class Scene {
public:
	Scene();
	~Scene();

	void addModel(const char* path, unsigned int& shaderProg);
	void addAnimatedModel(std::string path, unsigned int& shaderProg);
	int getModelCount() const;
	unsigned int getAnimModelCount() const;

	void addLight(glm::vec3 position, glm::vec3 color);
	int getLightCount() const;

	void CreateVAO(unsigned int& id);
	void CreateVBO(unsigned int& id);
	void CreateIBO(unsigned int& id);

	std::vector<RawModel> models;
	std::vector<AnimatedModel*> animatedModels;

	std::vector<Light> lights; //List of lights
	std::vector<glm::vec3> lightPositions; //List of all light positions
	std::vector<glm::vec3> lightColors; //List of all light colors

	AssimpLoader animLoader;

private:
	OBJLoader loader;
	int modelCount;
	unsigned int animatedModelCount;
	int lightCount;
};