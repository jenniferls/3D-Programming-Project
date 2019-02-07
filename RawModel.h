#pragma once
#include "glm/glm.hpp"
#include <vector>

class RawModel {
public:
	// this is how we will structure the input data for the vertex shader
	struct TriangleVertex {
		glm::vec3 positions;
		glm::vec2 UVs;
		glm::vec3 normals;
	};

	RawModel(int id, const char* filePath, const char* texPath);
	~RawModel();

	int getID() const;
	int getVertCount() const;
	void setVertCount(int count);
	const char* getPath() const;
	const char* getTexturePath() const;
	void setTexturePath(const char* texPath);
	void setMaterialPath(const char* matPath);
	const char* getMaterialPath() const;
	unsigned int getTextureID() const;
	void setTextureID(unsigned int id);

	unsigned int textureID;
	std::vector<TriangleVertex> vertices; //List of all vertices in model file
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned short> vertex_indices, uv_indices, normal_indices;

	//Used with MTL-files
	float ambientVal;
	float specularVal;
	float diffuseVal;

private:
	const char* path; //File path
	const char* texturePath; //File path to texture file
	const char* materialPath; //File path to .mtl-file
	int id;
	int vertCount;
};

