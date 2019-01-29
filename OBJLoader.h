#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "RawModel.h"

class OBJLoader {
public:
	OBJLoader();
	~OBJLoader();

	void loadOBJ(const char* filename, std::vector<glm::vec3> &vertPositions, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &uvs, std::vector<unsigned short> &elements);

private:
	RawModel model;
};

