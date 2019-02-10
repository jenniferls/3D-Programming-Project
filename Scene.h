#pragma once
#include <windows.h>
#include "glew/include/GL/glew.h"
#include <gl/GL.h>

#include <vector>
#include <string>
#include "RawModel.h"
#include "OBJLoader.h"
#include "Light.h"

class Scene {
public:
	Scene(unsigned int shaderProg);
	~Scene();

	void addModel(const char* path);
	int getModelCount() const;

	void addLight(glm::vec3 position, glm::vec3 color);
	void prepareLights();
	int getLightCount() const;

	GLuint CreateVAO();
	void deleteVAOs();

	std::vector<RawModel> models;
	std::vector<unsigned int> vaos;
	std::vector<Light> lights; //List of lights
	std::vector<glm::vec3> lightPositions; //List of all light positions
	std::vector<glm::vec3> lightColors; //List of all light colors

	int lights_pos_id;
	int lights_color_id;
private:
	OBJLoader loader;
	int modelCount;
	int lightCount;
	unsigned int shaderProg;
};

