#pragma once
#include "glm/glm.hpp"
#include <vector>

#define VERTEX_SIZE sizeof(RawModel::TriangleVertex)

class RawModel {
#define BUFFER_OFFSET(i) ((char *)nullptr + (i)) //To avoid redefinition in main
public:
	// this is how we will structure the input data for the vertex shader
	struct TriangleVertex {
		glm::vec3 positions;
		glm::vec2 UVs;
		glm::vec3 normals;
	};

	RawModel(const char* filePath);
	~RawModel();

	int getVertCount() const;
	void setVertCount(int count);
	const char* getPath() const;
	std::string getTexturePath() const;
	void setTexturePath(std::string texPath);
	void setMaterialPath(std::string matPath);
	std::string getMaterialPath() const;
	unsigned int getTextureID() const;
	void setTextureID(unsigned int id);

	glm::vec3 getWorldPosition() const;
	void setWorldPosition(glm::vec3 position);
	float getWorldRotation() const;
	void setWorldRotation(float rotation);

	unsigned int textureID;
	std::vector<TriangleVertex> vertices; //List of all vertices in model file
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned short> vertex_indices, uv_indices, normal_indices;

	//Used with MTL-files
	glm::vec3 ambientVal;
	glm::vec3 specularVal;
	glm::vec3 diffuseVal;

	unsigned int ambID;
	unsigned int specID;
	unsigned int diffID;

	unsigned int vaoID;
	unsigned int vboID;

	void prepare(unsigned int& shaderProgram);

	unsigned int shaderProg;

private:
	void prepareMaterials();

	const char* path; //File path
	std::string texturePath; //File path to texture file
	std::string materialPath; //File path to .mtl-file
	int vertCount;
	glm::vec3 worldPosition;
	float worldRotation;
};

