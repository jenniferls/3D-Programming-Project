#pragma once
#include <windows.h>
#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"
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
	Scene(unsigned int shaderProg, unsigned int shaderProgAnim);
	~Scene();

	void addModel(const char* path);
	void addAnimatedModel(std::string path);
	int getModelCount() const;
	unsigned int getAnimModelCount() const;

	void addLight(glm::vec3 position, glm::vec3 color);
	void prepareLights();
	int getLightCount() const;
	void prepareMaterials();
	void prepareJoints();

	GLuint CreateVAO();
	void deleteVAOs();
	GLuint CreateVBO();
	void deleteVBOs();
	GLuint CreateIBO();
	void deleteIBOs();

	std::vector<RawModel> models;
	std::vector<AnimatedModel> animatedModels;

	std::vector<unsigned int> vaos;
	std::vector<unsigned int> vbos;
	std::vector<unsigned int> ibos;

	std::vector<Light> lights; //List of lights
	std::vector<glm::vec3> lightPositions; //List of all light positions
	std::vector<glm::vec3> lightColors; //List of all light colors

	int lights_pos_id;
	int lights_color_id;
	int anim_lights_pos_id; //ID in FBX-shader
	int anim_lights_color_id; //ID in FBX-shader

	AssimpLoader animLoader;

private:
	OBJLoader loader;
	int modelCount;
	unsigned int animatedModelCount;
	int lightCount;
	unsigned int shaderProg;
	unsigned int shaderProgAnim;
};