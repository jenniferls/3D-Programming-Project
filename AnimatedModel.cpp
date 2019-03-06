#include "AnimatedModel.h"
#include <windows.h>
#include "glew/include/GL/glew.h"
#include <gl/GL.h>

AnimatedModel::AnimatedModel(std::string filePath, unsigned int shaderProg) {
	this->path = filePath;
	this->shaderProg = shaderProg;
	this->texturePath = "Path not loaded";

	this->vaoID = 0;
	this->vboID = 0;
	this->vboIDJoints = 0;
	this->iboID = 0;

	this->textureID = 0;
	this->ambID = 0;
	this->diffID = 0;
	this->specID = 0;

	this->vertCount = 0;
	this->jointCount = 0;
	this->numIndices = 0;

	this->worldPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->worldRotation = 0.0f;

	this->ambientVal = { 0.0f, 0.0f, 0.0f };
	this->diffuseVal = { 0.0f, 0.0f, 0.0f };
	this->specularVal = { 0.0f, 0.0f, 0.0f };

	this->scene = nullptr;

	prepareMaterials();
	prepareJoints();
}

AnimatedModel::~AnimatedModel() {
	OutputDebugStringA("Destructor is run for AnimatedModel\n");
	this->importer.FreeScene();
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
	glDeleteBuffers(1, &iboID);
}

void AnimatedModel::prepareMaterials() {
	this->ambID = glGetUniformLocation(this->shaderProg, "ambient_val"); //Assign ID
	this->diffID = glGetUniformLocation(this->shaderProg, "diffuse_val");
	this->specID = glGetUniformLocation(this->shaderProg, "specular_val");
}

void AnimatedModel::prepareJoints() {
	for (int i = 0; i < MAX_JOINTS; i++) {
		std::string name = "jointTransforms[" + std::to_string(i) + "]"; //Name in shader
		jointLocations[i] = glGetUniformLocation(this->shaderProg, name.c_str()); //Get an ID for all the possible joints to be used in shader
	}
}

void AnimatedModel::VertexJointData::addJointData(unsigned int id, float weight) {
	for (int i = 0; i < JOINTS_PER_VERTEX; i++) {
		if (weights[i] == 0.0f) {
			jointIDs[i] = id;
			weights[i] = weight;
			return;
		}
	}
}

std::string AnimatedModel::getPath() const {
	return this->path;
}

std::string AnimatedModel::getTexturePath() const {
	return this->texturePath;
}

void AnimatedModel::setTexturePath(std::string texPath) {
	this->texturePath = texPath;
}

void AnimatedModel::setVertCount(unsigned int count) {
	this->vertCount = count;
}

unsigned int AnimatedModel::getVertCount() const {
	return this->vertCount;
}

unsigned int AnimatedModel::getTextureID() const {
	return this->textureID;
}

void AnimatedModel::setTextureID(unsigned int id) {
	this->textureID = id;
}

glm::vec3 AnimatedModel::getWorldPosition() const {
	return this->worldPosition;
}

void AnimatedModel::setWorldPosition(glm::vec3 position) {
	this->worldPosition = position;
}

float AnimatedModel::getWorldRotation() const {
	return this->worldRotation;
}

void AnimatedModel::setWorldRotation(float rotation) {
	this->worldRotation = rotation;
}
