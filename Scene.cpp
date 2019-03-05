#include "Scene.h"

Scene::Scene(unsigned int shaderProg, unsigned int shaderProgAnim) {
	this->modelCount = 0;
	this->animatedModelCount = 0;
	this->lightCount = 0;
	this->shaderProg = shaderProg;
	this->shaderProgAnim = shaderProgAnim;
	this->lights_pos_id = -1;
	this->lights_color_id = -1;
	this->anim_lights_pos_id = -1;
	this->anim_lights_color_id = -1;
}


Scene::~Scene() {
	OutputDebugStringA("Destructor is run for scene\n");
	for (int i = 0; i < this->animatedModelCount; i++) {
		delete animatedModels[i];
	}
	animatedModels.clear();
}

void Scene::addModel(const char* path) {
	RawModel model(path);
	loader.loadOBJ(model); //Loads model from file
	loader.loadMTL(model); //Loads material from file
	models.push_back(model);
	this->modelCount++;
}

void Scene::addAnimatedModel(std::string path) {
	animatedModels.push_back(new AnimatedModel(path));
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

	anim_lights_pos_id = glGetUniformLocation(this->shaderProgAnim, "light_positions"); //Assign ID
	if (lights_pos_id == -1) {
		OutputDebugStringA("Error, cannot find 'lightpos_id' attribute in Fragment shader\n");
	}

	anim_lights_color_id = glGetUniformLocation(this->shaderProgAnim, "light_colors"); //Assign ID
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
	for (int i = 0; i < getAnimModelCount(); i++) {
		animatedModels[i]->ambID = glGetUniformLocation(this->shaderProgAnim, "ambient_val"); //Assign ID
		animatedModels[i]->diffID = glGetUniformLocation(this->shaderProgAnim, "diffuse_val");
		animatedModels[i]->specID = glGetUniformLocation(this->shaderProgAnim, "specular_val");
	}
}

void Scene::prepareJoints() {
	for (int i = 0; i < this->animatedModelCount; i++) {
		for (int j = 0; j < MAX_JOINTS; j++) {
			std::string name = "jointTransforms[" + std::to_string(j) + "]"; //Name in shader
			animatedModels[i]->jointLocations[j] = glGetUniformLocation(this->shaderProgAnim, name.c_str()); //Get an ID for all the possible joints to be used in shader
		}
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
