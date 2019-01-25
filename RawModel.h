#pragma once
#include "glm/glm.hpp"

class RawModel {
public:
	// this is how we will structure the input data for the vertex shader
	struct TriangleVertex {
		glm::vec3 positions;
		glm::vec2 UVs;
		glm::vec3 normals;
	};

	RawModel(int id, int vertCount);
	~RawModel();

	int getID() const;
	int getvertCount() const;

private:
	int id;
	int vertCount;
};

