#pragma once
#include <glm/glm.hpp>
#include <vector>

class AnimatedModel {
public:
	struct Vertex {
		glm::vec3 positions;
		glm::vec2 UVs;
		glm::vec3 normals;
	};

	AnimatedModel(std::string filePath);
	~AnimatedModel();

	std::string getPath() const;
	void setVertCount(unsigned int count);
	unsigned int getVertCount() const;

	unsigned int getVaoID() const;
	void setVaoID(unsigned int id);
	unsigned int getVboID() const;
	void setVboID(unsigned int id);
	unsigned int getIboID() const;
	void setIboID(unsigned int id);

	std::vector<Vertex> vertices; //List of all vertices in model file
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	unsigned int numIndices;

private:
	std::string path; //File path
	unsigned int vertCount;
	unsigned int vaoID;
	unsigned int vboID;
	unsigned int iboID; //Index buffer
};

