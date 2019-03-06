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

GLuint Scene::CreateVAO() {
	GLuint id = 0;
	// Vertex Array Object (VAO), description of the inputs to the GPU 
	glGenVertexArrays(1, &id);
	// bind is like "enabling" the object to use it
	glBindVertexArray(id);
	this->vaos.push_back(id);
	return id;
}

void Scene::deleteVAOs() {
	for (int i = 0; i < vaos.size(); i++) {
		glDeleteVertexArrays(1, &vaos[i]);
	}
}

GLuint Scene::CreateVBO() {
	GLuint vbo = 0;
	// create a vertex buffer object (VBO) id (out Array of Structs on the GPU side)
	glGenBuffers(1, &vbo);
	// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	this->vbos.push_back(vbo);
	return vbo;
}

void Scene::deleteVBOs() {
	for (int i = 0; i < vbos.size(); i++) {
		glDeleteBuffers(1, &vbos[i]);
	}
}

GLuint Scene::CreateIBO() {
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	this->ibos.push_back(ibo);
	return ibo;
}

void Scene::deleteIBOs() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	for (int i = 0; i < ibos.size(); i++) {
		glDeleteBuffers(1, &ibos[i]);
	}
}
