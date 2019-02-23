#include "Scene.h"

Scene::Scene(unsigned int shaderProg) {
	this->modelCount = 0;
	this->animatedModelCount = 0;
	this->lightCount = 0;
	this->shaderProg = shaderProg;
	this->lights_pos_id = -1;
	this->lights_color_id = -1;
}


Scene::~Scene() {

}

void Scene::addModel(const char* path) {
	RawModel model(path);
	loader.loadOBJ(model); //Loads model from file
	loader.loadMTL(model); //Loads material from file
	models.push_back(model);
	this->modelCount++;
}

void Scene::addAnimatedModel(std::string path) {
	AnimatedModel model(path); //Create a model
	animLoader.LoadModel(model);//Load the model
	////Load the texture
	animatedModels.push_back(model); //Push back the model for rendering
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

void Scene::prepareMaterials() {
	for (int i = 0; i < getModelCount(); i++) {
		models[i].ambID = glGetUniformLocation(this->shaderProg, "ambient_val"); //Assign ID
		models[i].diffID = glGetUniformLocation(this->shaderProg, "diffuse_val");
		models[i].specID = glGetUniformLocation(this->shaderProg, "specular_val");
	}
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

	return vbo;
}

void Scene::deleteVBOs() {
	for (int i = 0; i < vbos.size(); i++) {
		glDeleteBuffers(1, &vbos[i]);
	}
}
