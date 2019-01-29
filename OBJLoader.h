#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "RawModel.h"

class OBJLoader {
public:
	OBJLoader();
	~OBJLoader();

	bool loadOBJ(const char* filename, std::vector<glm::vec3> &vertPositions, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &uvs,
		std::vector<unsigned short> &vert_indices, std::vector<unsigned short> &uv_indices, std::vector<unsigned short> &norm_indices);

private:
	RawModel model;
};

