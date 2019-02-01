#pragma once
#include "glm/glm.hpp"
#include <vector>
#include <string>

class RawModel {
public:
	// this is how we will structure the input data for the vertex shader
	struct TriangleVertex {
		glm::vec3 positions;
		glm::vec2 UVs;
		glm::vec3 normals;
	};

	RawModel(int id, std::string filePath);
	~RawModel();

	int getID() const;
	int getVertCount() const;
	void setVertCount(int count);

	const char* path; //File path
	std::vector<TriangleVertex> vertices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned short> vertex_indices, uv_indices, normal_indices;

private:
	int id;
	int vertCount;
};

